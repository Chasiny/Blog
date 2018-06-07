# sync.Map源码分析

## 普通的map
go普通的map是不支持并发的，例如简单的写
```go
func main() {
	wg := sync.WaitGroup{}
	wg.Add(10)

	m := make(map[int]int)

	for i := 0; i < 10; i++ {
		go func(i int) {
			m[i] = i
			wg.Done()
		}(i)
	}

	wg.Wait()
}
```
```bash
fatal error: concurrent map writes
```

## sync.Map

go的sync.Map几个优化点
* 通过使用优先读的结构体read减少锁的冲突
* 使用双重检测
* 使用延迟删除（删除存在于read中的数据只是将其置为nil）
* 动态调整，miss次数多了之后，将dirty数据提升为read


从sync/map.go看Map的结构体
```go
type Map struct {
	mu Mutex                        //互斥锁，用于锁定dirty map

	read atomic.Value               //读map，实际上不是只读，是优先读
	dirty map[interface{}]*entry    //dirty是一个当前最新的map，允许读写

	misses int                      //标记在read中没有命中的次数，当misses等于dirty的长度时，会将dirty复制到read
}

//read存储的实际结构体
type readOnly struct {
	m       map[interface{}]*entry      //map
	amended bool                        //如果有些数据在dirty中但没有在read中，该值为true
}

type entry struct {
	p unsafe.Pointer                //数据指针
}
```

### entry的几种类型
* nil: 表示为被删除，此时read跟dirty同时有该键（一般该键值如果存在于read中，则删除是将其标记为nil）
* expunged: 也是表示被删除，但是该键只在read而没有在dirty中，这种情况出现在将read复制到dirty中，即复制的过程会先将nil标记为expunged，然后不将其复制到dirty
* 其他: 表示存着真正的数据


## sync.Map几种方法：  

首先先说明read跟dirty不是直接存对象，而是存指针，这样的话如果键值同时存在在read跟dirty中，直接原子修改read也相当于修改dirty中的值，并且当read跟dirty存在大量相同的数据时，也不会使用太多的内存

### Load
```go
func (m *Map) Load(key interface{}) (value interface{}, ok bool) {
	read, _ := m.read.Load().(readOnly)
	e, ok := read.m[key]
	if !ok && read.amended {
        //如果不在read中，并且dirty有新数据，则从dirty拿
        
        m.mu.Lock()
        
        //双重检查，因为有可能在加锁前read刚好插入该值
		read, _ = m.read.Load().(readOnly)
		e, ok = read.m[key]
		if !ok && read.amended {
            //没有在read中，则从dirty拿

			e, ok = m.dirty[key]
			m.missLocked()
		}
		m.mu.Unlock()
	}
	if !ok {
		return nil, false
	}
	return e.load()
}

func (m *Map) missLocked() {
    //没有命中的计数加一
	m.misses++
	if m.misses < len(m.dirty) {
		return
    }
    //当没有命中的次数等于dirty的大小，将dirty复制给read
	m.read.Store(readOnly{m: m.dirty})
	m.dirty = nil
	m.misses = 0
}
```
read主要用于读取，每次Load都先从read读取，当read中不存在且amended为true，就从dirty读取数据  
无论dirty是否存在该key，都会执行missLocked函数，该函数将misses+1，当misses等于dirty的大小时，便会将dirty复制到read，此时再将dirty置为nil  


### Delete
```go
func (m *Map) Delete(key interface{}) {
	read, _ := m.read.Load().(readOnly)
	e, ok := read.m[key]
	if !ok && read.amended {
        //如果不在read中，并且dirty有新数据，则从dirty中找

        m.mu.Lock()
        
        //双重检查
		read, _ = m.read.Load().(readOnly)
		e, ok = read.m[key]
		if !ok && read.amended {
            //这是表示键值只存在于dirty，直接删除dirty中的键值即可
			delete(m.dirty, key)
		}
		m.mu.Unlock()
    }
    
	if ok {
        //如果在read中，则将其标记为删除（nil）
		e.delete()
	}
}

func (e *entry) delete() (hadValue bool) {
	for {
		p := atomic.LoadPointer(&e.p)
		if p == nil || p == expunged {
			return false
		}
		if atomic.CompareAndSwapPointer(&e.p, p, nil) {
			return true
		}
	}
}
```
先判断是否在read中，不在的话再从dirty删除

### Store
```go
func (m *Map) Store(key, value interface{}) {
    //如果read存在这个键，并且这个entry没有被标记删除，尝试直接写入
	//dirty也指向这个entry，所以修改e也可以使dirty也保持最新的entry
	read, _ := m.read.Load().(readOnly)
	if e, ok := read.m[key]; ok && e.tryStore(&value) {
		return
	}

	m.mu.Lock()
	read, _ = m.read.Load().(readOnly)
	if e, ok := read.m[key]; ok {
        //该键值存在在read中

		if e.unexpungeLocked() {
            //该键值在read中被标记为抹除，则将其添加到dirty

			m.dirty[key] = e
        }
        
        //更新entry
        e.storeLocked(&value)
        
	} else if e, ok := m.dirty[key]; ok {

        //如果不在read中，在dirty中，则更新
        e.storeLocked(&value)
        
	} else {
        //既不在read中，也不在dirty中

		if !read.amended {
            //从read复制没有标记删除的数据到dirty中
			m.dirtyLocked()
			m.read.Store(readOnly{m: read.m, amended: true})
        }
        
        //添加到dirty中
		m.dirty[key] = newEntry(value)
	}
	m.mu.Unlock()
}

func (e *entry) tryStore(i *interface{}) bool {
	p := atomic.LoadPointer(&e.p)
	if p == expunged {
		return false
	}
	for {
		if atomic.CompareAndSwapPointer(&e.p, p, unsafe.Pointer(i)) {
			return true
		}
		p = atomic.LoadPointer(&e.p)
		if p == expunged {
			return false
		}
	}
}

func (e *entry) unexpungeLocked() (wasExpunged bool) {
	return atomic.CompareAndSwapPointer(&e.p, expunged, nil)
}

func (e *entry) storeLocked(i *interface{}) {
	atomic.StorePointer(&e.p, unsafe.Pointer(i))
}

func (m *Map) dirtyLocked() {
	if m.dirty != nil {
		return
	}

    //从read复制到dirty
	read, _ := m.read.Load().(readOnly)
	m.dirty = make(map[interface{}]*entry, len(read.m))
	for k, e := range read.m {

        //如果标记为nil或者expunged，则不复制到dirty
		if !e.tryExpungeLocked() {
			m.dirty[k] = e
		}
	}
}

func (e *entry) tryExpungeLocked() (isExpunged bool) {
	p := atomic.LoadPointer(&e.p)
	for p == nil {
        //尝试将nil置为expunged
		if atomic.CompareAndSwapPointer(&e.p, nil, expunged) {
			return true
		}
		p = atomic.LoadPointer(&e.p)
	}
	return p == expunged
}
```

sync.Map 写入就稍微麻烦很多了  
1. 首先会先判断键值是否已经存在read中，存在的话便尝试直接写入（read不只是读，此时被写入），由于从read获取的是entry指针，因此对从read读取entry进行修改便相当于修改dirty中对应的entry，此时写入的是使用原子操作。
2. 键值存在在read中并且该entry被标记为expunged（这种情况出现在从read复制数据到dirty中，看tryExpungeLocked函数，将所有键为nil置为expunged，表示该键被删除，但没有在dirty中）
3. 从read复制到dirty的过程来说，主要是用dirtyLocked函数实现的，复制除了entry为nil跟expunged的数据




---
参考
* [Go sync.Map](https://github.com/golang/go/blob/master/src/sync/map.go)
* [Go 1.9 sync.Map揭秘](http://colobu.com/2017/07/11/dive-into-sync-Map/)