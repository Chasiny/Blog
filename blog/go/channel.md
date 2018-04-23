# go channel 详解

# channel分类
go的channel分为有缓冲channel和无缓冲channel，如下：
```go
	unBufferedChan := make(chan int)		//无缓冲通道
	bufferedChan := make(chan int, 10)		//有缓冲通道
```
# 用法
## 发送数据
先看一个简单的例子，这是个无缓冲的通道
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
很明显，当主协程发送完一个数据后，需要等待通道的下游接收，不然会处于堵塞状态   
当我们将
```go
testChan := make(chan int)
```
改成
```go
testChan := make(chan int, 5)
```
结果是
```
main goroutine send :  0
main goroutine send :  1
main goroutine send :  2
main goroutine send :  3
main goroutine send :  4
main goroutine send :  5
child goroutine receive :  0
main goroutine send :  6
child goroutine receive :  1
main goroutine send :  7
child goroutine receive :  2
main goroutine send :  8
child goroutine receive :  3
main goroutine send :  9
child goroutine receive :  4
child goroutine receive :  5
child goroutine receive :  6
child goroutine receive :  7
child goroutine receive :  8
child goroutine receive :  9
```
这时主协程不需要等待通道下游取数据后才能发送，而是通道有一定的缓冲，主协程能连续发送多个数据直到缓冲区满了

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