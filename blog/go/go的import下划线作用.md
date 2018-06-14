# 
如果我们导入一个包只是想利用导入包而产生的副作用，而不真正使用该包，那么我们就可以使用下划线 _ 来重命名
导入的包。像往常一样，下划线 _ 为空白标识符，并不能被访问。

```go
import (
	_ "stl/vector"
	"fmt"
)

func main(){
	fmt.Println("main function")
}
```

输出
```
this is vector' init function
main function
```

但是我们如果使用下划线 _ 作为该包，那么该包便是无法使用的
```go
import (
	_ "stl/vector"
	"fmt"
)

func main(){
	vector.Vector{}

	fmt.Println("main function")
}
```
输出
```
.\main.go:9:2: undefined: vector
```
这里很容易理解，因为我们将vector包重命名为匿名包，那么main函数自然找不到vector包了，当然，如果我们需要调用到vector包，那就不能用将该包声明为匿名

