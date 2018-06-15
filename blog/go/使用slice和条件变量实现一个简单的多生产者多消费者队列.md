# 使用slice和条件变量实现一个简单的多生产者多消费者队列

[项目代码](../../code/go/message_queue)

## 设计要求
* 多生产者，多消费者   
* 生产时若缓冲区以满，则写入阻塞   
* 消费时若缓冲区为空，则读取等待

## 设计思路

### 结构体
首先定义队列结构体
```go
type MessageQueue struct {

	msgdata []interface{}		//缓冲区
	len     int32				//缓冲区长度

	readPos int32				//读取指向的指针
	readMutex sync.Mutex		//读取锁

	writePos int32				//写入指向的指针
	writeMutex sync.Mutex		//写入锁

	emptyCond *sync.Cond		//缓冲区为空条件变量
	fullCond  *sync.Cond		//缓冲区为满条件变量
}
```
这里使用的缓冲区是一个环形队列

### 写入的方法(Put)

```go
func (mq *MessageQueue) Put(in interface{}) {
	//首先获取写锁，所有写入的优先级是一样的
	mq.writeMutex.Lock()
	defer mq.writeMutex.Unlock()

	//判断缓冲区是否为满
	mq.fullCond.L.Lock()
	defer mq.fullCond.L.Unlock()
	for (mq.writePos+1)%mq.len == mq.readPos {
		//缓冲区为满，等待消费者消费的通知缓冲区有数据被取出
		mq.fullCond.Wait()
	}

	//写入一个数据
	mq.msgdata[mq.writePos] = in
	mq.writePos = (mq.writePos + 1) % mq.len

	//通知消费者已经有缓冲区有数据了
	mq.emptyCond.Signal()
}
```

### 读取的方法(Get)
```go
func (mq *MessageQueue) Get() (out interface{}) {
	//获取读锁，读取的优先级也是一样的
	mq.readMutex.Lock()
	defer mq.readMutex.Unlock()

	//判断缓冲区是否为空
	mq.emptyCond.L.Lock()
	defer mq.emptyCond.L.Unlock()

	for mq.writePos == mq.readPos {
		//缓冲区为空，等待生产者通知缓冲区有数据存入
		mq.emptyCond.Wait()
	}

	//读取
	out = mq.msgdata[(mq.readPos)%mq.len]
	mq.readPos = (mq.readPos + 1) % mq.len

	//通知生产者已经有缓冲区有空间了
	mq.fullCond.Signal()

	return
}
```

### New方法(New)
```go
func NewMQ(len int32) *MessageQueue {
	if len < 1 {
		panic("new meg queue fail: len < 1")
		return nil
	}

	l:=&sync.Mutex{}

	return &MessageQueue{
		msgdata:  make([]interface{}, len+1),
		len:      len + 1,
		readPos:  0,
		writePos: 0,

		emptyCond: sync.NewCond(l),
		fullCond:  sync.NewCond(l),
	}
}
```

### 长度(Len)
```go
func (mq *MessageQueue) Len() int32 {
	if mq.writePos < mq.readPos {
		return mq.writePos + mq.len - mq.readPos
	}

	return mq.writePos - mq.readPos
}
```