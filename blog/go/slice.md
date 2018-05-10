# slice（切片）
go中的slice是对数组一个连续片段的引用，因此切片是一个引用类型
  
## 结构
slice的结构定义如下（runtime/slice.go）
```go
type slice struct {
	array unsafe.Pointer
	len   int
	cap   int
}
```
* array表示切片指向引用数组的第一个数据的地址
* len表示切片的长度
* cap表示切片的最大容量



## 切片的扩容
当切片经过append后超出最大容量cap后，会对slice进行扩容  
在go的源码中（runtime/slice.go），growslice函数是用于扩容切片的，参考如下
```go
func growslice(et *_type, old slice, cap int) slice { 
	//...

	newcap := old.cap
	doublecap := newcap + newcap
	if cap > doublecap {
		newcap = cap
	} else {
		if old.len < 1024 {
			newcap = doublecap
		} else {
			// Check 0 < newcap to detect overflow
			// and prevent an infinite loop.
			for 0 < newcap && newcap < cap {
				newcap += newcap / 4
			}
			// Set newcap to the requested cap when
			// the newcap calculation overflowed.
			if newcap <= 0 {
				newcap = cap
			}
		}
	}

	//...

	return slice{p, old.len, newcap}
}
```
其扩容后长度规则取决如下
* 如果原始长度小于1024，扩容后的长度为原来的两倍
* 如果原始长度大于1024，则扩容以原来1/4增长，直到大于期待的长度（growslice的参数cap）
* 如果增长后的长度溢出，则用期待的长度来代替扩容后的长度

