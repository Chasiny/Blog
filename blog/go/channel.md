# go channel 详解

# channel分类
go的channel分为有缓冲channel和无缓冲channel，如下：
```go
	unBufferedChan := make(chan int)		//无缓冲通道
	bufferedChan := make(chan int, 1)		//有缓冲通道
```

## 有无缓冲的区别
首先笔者刚学chan也是以为make(chan int)是跟make(chan int, 1)是一样的，这样的理解是错误的，例如给个简单的例子：
```
func main(){
	c:=make(chan int)

	c<-1
	fmt.Println(<-c)
}
```
这样的情况是不会打印出`1`的，并且是进入死锁状态：
```
fatal error: all goroutines are asleep - deadlock!
```
再看make(chan int, 1)：
```go
func main(){
	c:=make(chan int,1)

	c<-1
	fmt.Println(<-c)
}
```
结果就是打印了`1`。  

那么有无缓冲的最大区别是什么，那就是一个是同步的 一个是非同步的。
* 例如刚才第一个例子的make(chan int)，当塞一个数据进chan时，需要chan的下游接收后才会继续往下执行，否则会一直阻塞在这里，而取数据是在写入数据后才执行的，因此会进入死锁的僵局
* 第二个例子，由于chan有容量为1的缓冲区，因此写入一个数据后可以暂存在缓冲区，可以继续往下执行，执行到fmt.Println(<-c)取出数据，因此可以正常打印数据而不是进入死锁的僵局

# chan的几种异常使用

## 对于关闭后的chan

* 对关闭后的chan执行写入：
```go
func main(){
	c:=make(chan int,1)
	close(c)
	c<-1
}
```
导致panic
```go
panic: send on closed channel
```

* 对关闭后的chan执行读取：
```go
func main(){
	c:=make(chan int,1)
	close(c)
	fmt.Println(<-c)
}
```
输出0值
```go
0
```

# chan用法
## 发送数据
```go
package main

import (
	"fmt"
	"sync"
	"time"
)

func main() {
	wg := sync.WaitGroup{}
	wg.Add(1)
	testChan := make(chan int)
	go func() {
	Exit:
		for {
			select {
			case data := <-testChan:
				time.Sleep(time.Millisecond)
				fmt.Println("child goroutine receive : ", data)
				wg.Done()
				continue Exit
			default:
			}

		}
	}()

	for i := 0; i < 10; i++ {
		testChan <- i
		fmt.Println("main goroutine send : ", i)
		wg.Add(1)
	}

	wg.Done()
	wg.Wait()
}
```
结果是
```
main goroutine send :  0
child goroutine receive :  0
main goroutine send :  1
child goroutine receive :  1
main goroutine send :  2
child goroutine receive :  2
main goroutine send :  3
child goroutine receive :  3
main goroutine send :  4
child goroutine receive :  4
main goroutine send :  5
child goroutine receive :  5
main goroutine send :  6
child goroutine receive :  6
main goroutine send :  7
child goroutine receive :  7
main goroutine send :  8
child goroutine receive :  8
main goroutine send :  9
child goroutine receive :  9
```

## 同步信号
channel可以当做同步信号，当关闭一个channel时，channel所有下游都会受到一个空值，当主协程需要退出时，可以用channel通知子协程，让子协程退出
```go
package main

import (
	"fmt"
	"sync"
)

func main() {
	exitChan := make(chan struct{})
	wg := sync.WaitGroup{}
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(t int) {
			<-exitChan
			fmt.Println("child goroutine ", t, " exit")
			wg.Done()
		}(i)
	}

	close(exitChan)
	wg.Wait()
	fmt.Println("main goroutine exit")
}
```
结果是
```
child goroutine  0  exit
child goroutine  6  exit
child goroutine  4  exit
child goroutine  7  exit
child goroutine  8  exit
child goroutine  9  exit
child goroutine  2  exit
child goroutine  3  exit
child goroutine  1  exit
child goroutine  5  exit
main goroutine exit
```

## 生产者消费者
channel也可以用于生产者消费者模型，例如一个协程用于产生数据，另一个协程用于消费数据，消费者不需要等生产者全部生产完数据再消费，例如
```go
package main

import (
	"fmt"
	"time"
)

func Productor() <-chan int {
	dataChan := make(chan int, 5)
	go func() {
		for i := 10; i < 20; i++ {
			fmt.Println("Productor create data : ", i)
			dataChan <- i
			time.Sleep(time.Microsecond)
		}
	}()
	return dataChan
}

func main() {
	productor := Productor()
	for i := 0; i < 10; i++ {
		data := <-productor
		fmt.Println("Consumer receive data : ", data)
	}
}
```
结果如下
```
Productor create data :  10
Consumer receive data :  10
Productor create data :  11
Consumer receive data :  11
Productor create data :  12
Consumer receive data :  12
Productor create data :  13
Consumer receive data :  13
Productor create data :  14
Consumer receive data :  14
Productor create data :  15
Consumer receive data :  15
Productor create data :  16
Consumer receive data :  16
Productor create data :  17
Consumer receive data :  17
Productor create data :  18
Consumer receive data :  18
Productor create data :  19
Consumer receive data :  19
```