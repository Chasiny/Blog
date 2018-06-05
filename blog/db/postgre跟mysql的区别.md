postgres:
* 支持array，json，可以在 array 和 json 上建索引,
* 支持表达式索引，部分索引
* MySQL 不支持 OVER 子句, 而 Pg 支持
* 在高并发读写，负载逼近极限下，PG的性能指标仍可以维持双曲线甚至对数曲线，到顶峰之后不再下降，而 MySQL 明显出现一个波峰后下滑