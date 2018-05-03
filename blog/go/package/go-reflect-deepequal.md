# DeepEqual
## 规则

### 1.不同类型的值不会深度相等
Values of distinct types are never deeply equal.
```go
type S1 struct {
	Field int
}
type S2 struct {
	Field int
}

func main() {
	fmt.Println(reflect.DeepEqual(S1{1}, S2{1}))
}
```
>对应输出false

### 2.当两个数组的元素对应深度相等时，两个数组深度相等
Array values are deeply equal when their corresponding elements are deeply equal.
```go
func main() {
	Array1 := []string{"hello1", "hello2"}
	Array2 := []string{"hello1", "hello2"}

	fmt.Println(reflect.DeepEqual(Array1, Array2))
}
```
>对应输出true

### 3.当两个相同结构体的所有字段对应深度相等的时候，两个结构体深度相等
Struct values are deeply equal if their corresponding fields,both exported and unexported, are deeply equal.
```go
type S struct {
	Field1 int
	field2 string
}

func main() {
	s1 := S{Field1: 1, field2: "hello"}
	s2 := S{Field1: 1, field2: "hello"}
	fmt.Println(reflect.DeepEqual(s1, s2))
}
```
>对应输出true

### 4.当两个函数都为nil时，两个函数深度相等，其他情况不相等（相同函数也不相等）
Func values are deeply equal if both are nil; otherwise they are not deeply equal.
```go

func main() {
	f1 := func(a int) int {
		return a * 2
	}
	fmt.Println(reflect.DeepEqual(f1, f1))
	f1 = nil
	fmt.Println(reflect.DeepEqual(f1, f1))
}
```
>对应输出false跟true

### 5.当两个interface的真实值深度相等时，两个interface深度相等
Interface values are deeply equal if they hold deeply equal concrete values.

```go
func main() {
	var i1 interface{}
	i1 = "hello"
	var i2 interface{}
	i2 = "hello"
	fmt.Println(reflect.DeepEqual(i1, i2))
}
```
>对应输出true


### 6.go中map的比较需要同时满足以下几个
* 1.两个map都为nil或者都不为nil，并且长度要相等  
they are both nil or both non-nil, they have the same length
* 2.相同的map对象或者所有key要对应相同
either they are the same map object or their corresponding keys
* 3.map对应的value也要深度相等
map to deeply equal values

```go
func main() {
	m1 := map[string]int{
		"a": 1,
		"b": 2,
	}
	m2 := map[string]int{
		"a": 1,
		"b": 2,
	}
	fmt.Println(reflect.DeepEqual(m1, m2))
}
```
>对应输出true

### 7.指针，满足以下其一即是深度相等
* 1.两个指针满足go的==操作符
Pointer values are deeply equal if they are equal using Go's == operator
* 2.两个指针指向的值是深度相等的

```go
func main() {
	m1 := map[string]int{
		"a": 1,
		"b": 2,
	}
	m2 := map[string]int{
		"a": 1,
		"b": 2,
	}
	M1:=&m1
	M2:=&m2
	fmt.Println(reflect.DeepEqual(M1, M2))
}
```
>对应输出true

### 8.切片，需要同时满足以下几点才是深度相等
* 1.两个切片都为nil或者都不为nil，并且长度要相等  
they are both nil or both non-nil, they have the same length
* 2.两个切片底层数据指向的第一个位置要相同或者底层的元素要深度相等
either they point to the same initial entry of the same underlying array (that is, &x[0] == &y[0]) or their corresponding elements (up to length) are deeply equal.
```go
func main() {
	s1 :=[] int {1,2,3,4,5}
	s2 :=s1[0:3]
	s3 :=s1[0:3]
	fmt.Println(reflect.DeepEqual(s2, s3))
	s4 :=s1[1:4]
	fmt.Println(reflect.DeepEqual(s2, s4))
}
```
>对应输出true,false

#### 注：空的切片跟nil切片是不深度相等的，例如
```go
func main() {
	s1 :=[]byte{}
	s2 :=[]byte(nil)
	fmt.Println(reflect.DeepEqual(s1, s2))
}
```
>对应输出false