
# Go逃逸分析(TODO)
## 什么是逃逸
* 当一个变量(或对象)在子程序中被分配时，一个指向变量的指针可能逃逸到其它执行线程中，或者去调用子程序。如果使用尾递归优化（通常在函数编程语言中是需要的），对象也可能逃逸到被调用的子程序中。如果一种语言支持第一类型的延续性在Scheme和Standard ML of New Jersey中同样如此），部分调用栈也可能发生逃逸。
* 如果一个子程序分配一个对象并返回一个该对象的指针，该对象可能在程序中的任何一个地方被访问到——这样指针就成功“逃逸”了。如果指针存储在全局变量或者其它数据结构中，它们也可能发生逃逸，这种情况是当前程序中的指针逃逸。

逃逸分析需要确定指针所有可以存储的地方，保证指针的生命周期只在当前进程或线程中。

## 逃逸分析的用处
* 编译器可以使用逃逸分析的结果作为优化基础
* 将堆分配转化为栈分配（Go），如果一个对象在子程序中被分配，要使指向该对象的指针永远不会逃逸，对象可能是栈分配的候选，而不是堆分配（减少GC压力）。
* 同步省略。如果一个对象被发现只能从一个线程被访问到，那么对于这个对象的操作可以不考虑同步。
* 分离对象或标量替换。有的对象可能不需要作为一个连续的内存结构存在也可以被访问到，那么对象的部分（或全部）可以不存储在内存，而是存储在CPU寄存器中。

## GO开启编译时逃逸分析日志
启动逃逸分析日志的命令只需要在编译的时候添加参数  -gcflags '-m'
   

# 逃逸的几种情况
* 随着Go版本的不断更新，Go的逃逸分析也逐渐在完善，下面介绍的几种逃逸情况,基于go-version:1.10
### 样例1：闭包函数调用 
> ```go
> 	var y int
> 	func(p *int, x int) {
> 		*p = x
> 	}(&y, 42)
> 
> 	x := 0
> 	defer func(p *int) {
> 		*p = 1
> 	}(&x)
> ```
> 结果分析：
> ```
> ./main.go:6:2: can inline main.func1
> ./main.go:8:3: inlining call to main.func1
> ./main.go:11:8: can inline main.func2
> ./main.go:13:4: &x escapes to heap
> ./main.go:10:2: moved to heap: x
> ./main.go:8:4: main &y does not escape
> ./main.go:11:8: main func literal does not escape
> ./main.go:11:16: main.func2 p does not escape
> ```
> 这是因为GO的逃逸分析并不会对闭包函数调用进行分析

### 样例2：赋值给间接指针
> ```go
>   i := 0
>   pp := new(*int)
>   *pp = &i
>   _ = pp
> ```
> 结果分析：
> ```
> ./main.go:10:8: &i escapes to heap
> ./main.go:8:2: moved to heap: i
> ./main.go:9:11: main new(*int) does not escape
> ```
> 再通过下面这两个例子对比更加明显  
> ```go
> 	i := 0
> 	var v X
> 	v.p = &i
> ```
> 结果分析：
> ```
> ./main.go:14:8: main &i does not escape
> ```
> 这次不会发生逃逸，而下面这个例子则发生了逃逸
> ```go
> 	y := new(X)
> 	y.p = &i 
> ```
> 结果分析：
> ```
> ./main.go:16:8: &i escapes to heap
> ./main.go:12:2: moved to heap: i
> ./main.go:15:10: main new(X) does not escap
> ```


### 样例3：赋值给slices或者maps指针
> ```go
> 	m := make(map[int]*T) 	//没有逃逸
> 	m[0] = new(T)           //发生逃逸
> ```
> 结果分析：
> ```
> .\main.go:11:36: leaking param: arg to result ~r1 level=1
> .\main.go:17:12: new(T) escapes to heap
> .\main.go:16:11: main make(map[int]*T) does not escape
> ```

