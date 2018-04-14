
# redis字典实现
### 哈希表
```c
typedef struct dictht{
    dictEntry ** table;     //哈希表数组
    unsigned long size;     //哈希表大小
    unsigned long sizemask; //哈希表大小掩码，用于计算索引值，总是等于size-1
    unsigned long used;     //该哈希表已有节点的数量
}dictht;
```
其中主要是table用于存放数据，其是一个dictEntry指针数组

### 哈希表节点
```c
typedef struct dictEntry{
    void *key;      //键
    union{
        void *val;
        uint64_tu64;
        int64_ts64;
    }v;             //值
    struct dictEntry *next;
}dictEntry;
```
## 字典的实现
```c
typedef struct dictEntry{
    dictType *type;     //类型特定函数
    void *privdata;     //私有数据
    dictht ht[2];       //哈希表
    int rehashidx;      //rehash索引，当rehash不在进行时，值为-1
}dict;
```
* 其中的type属性和privdata属性是针对不同类型的键值对，为创建多态字典而设置的。
```c
typedef struct dictEntry{
    unsigned int (*hashFunction)(const void *key);                              //计算哈希值的函数
    void *(*keyDup)(void *privdata,const void *key);                            //复制键的函数
    void *(*valDup)(void *privdata,const void *obj);                            //复制值的函数
    int (*keyCompare)(void *privdata,const void *key1,const void *key2);        //对比键的函数
    void *(*keyDestructor)(void *privdata,const void *key);                     //销毁键的函数
    void *(*valDestructor)(void *privdata,const void *obj);                     //销毁值的函数
}
```
* ht是个长度为2的数组，一般情况下，字典只使用ht[0]，ht[1]只在rehash时使用
* rehashidx也跟rehash有关，记录了rehash的进度。如果当前没有进行rehash，则值为-1

### 哈希算法---
title: redis字典
date: 2018-03-28 20:27:21
tags:
categories: 
- redis
---

### 哈希表
```c
typedef struct dictht{
    dictEntry ** table;     //哈希表数组
    unsigned long size;     //哈希表大小
    unsigned long sizemask; //哈希表大小掩码，用于计算索引值，总是等于size-1
    unsigned long used;     //该哈希表已有节点的数量
}dictht;
```
其中主要是table用于存放数据，其是一个dictEntry指针数组

### 哈希表节点
```c
typedef struct dictEntry{
    void *key;      //键
    union{
        void *val;
        uint64_tu64;
        int64_ts64;
    }v;             //值
    struct dictEntry *next;
}dictEntry;
```
## 字典的实现
```c
typedef struct dictEntry{
    dictType *type;     //类型特定函数
    void *privdata;     //私有数据
    dictht ht[2];       //哈希表
    int rehashidx;      //rehash索引，当rehash不在进行时，值为-1
}dict;
```
* 其中的type属性和privdata属性是针对不同类型的键值对，为创建多态字典而设置的。
```c
typedef struct dictEntry{
    unsigned int (*hashFunction)(const void *key);                              //计算哈希值的函数
    void *(*keyDup)(void *privdata,const void *key);                            //复制键的函数
    void *(*valDup)(void *privdata,const void *obj);                            //复制值的函数
    int (*keyCompare)(void *privdata,const void *key1,const void *key2);        //对比键的函数
    void *(*keyDestructor)(void *privdata,const void *key);                     //销毁键的函数
    void *(*valDestructor)(void *privdata,const void *obj);                     //销毁值的函数
}
```
* ht是个长度为2的数组，一般情况下，字典只使用ht[0]，ht[1]只在rehash时使用
* rehashidx也跟rehash有关，记录了rehash的进度。如果当前没有进行rehash，则值为-1

### 哈希算法
当有新的键值要插入字典时，程序会对键进行hash值计算，然后计算其索引值，其步骤如下
* 计算hash：hash=dict->type->hashFunction(key);
* 计算索引：indix=hash & dict->ht[x].sizemask;  
redis使用的是MurmurHash2来计算键的哈希值

### 解决键的冲突
redis采用链地址法来解决键的冲突，即每个哈希表节点有一个next指针，多个哈希表节点可以使用next指针构成一个单链表，被分配到同一个索引的多个节点就可以使用这个单链表连接起来，以解决键的冲突。

### rehash（重新散列）
rehash是对哈希表进行相应的扩展和收缩，即哈希表重新分配内存的过程，以维持哈希表的负载因子在一个合理的范围
redis对字典的哈希表rehash步骤如下
1. 为字典的ht[1]哈希表分配空间，其分配的大小取决于执行的操作以及ht[0]当前包含的键值对的数量（ht[0].used的值）
> * 当是扩展操作，则ht[1]的大小为第一个大于等于ht[0].used*2的2^n（2的n次幂）
> * 当执行的时收缩操作，则ht[1]的大小为第一个大于等于ht[0].used的2^n
2. 将保存在ht[0]中的所有键值对rehash到ht[1]上面
3. 当ht[0]包含的所有键值对迁移到ht[1]之后（ht[0]变为空表），释放ht[0],将ht[1]设置为ht[0]，并在ht[1]新创键一个空哈希表，为下次rehash做准备

### 哈希表的负载因子
负载因子=哈希表已保存节点数量/哈希表大小  
load_factor=ht[0].used/ht[0].size

### 哈希表的扩展与收缩
扩展  
1. 当服务器目前没有执行BGSAVE或者BGREWRITEAOF命令，且哈希表的负载因子大于等于1
2. 服务器目前正在执行BGSAVE或者BGREWRITEAOF命令,并且哈希表的负载因子大于等于5
* 在执行BGSAVE或者BGREWRITEAOF命令过程中，redis需要创建当前服务器进程的子进程，而大多数操作系统采用写时复制技术来优化子进程的使用效率，所以在子进程存在期间，服务器会提高执行扩展操作所需要的负载因子，从而避免在子进程存在期间进行哈希表扩展的操作，这可以避免不必要的内存写入，最大限度节约内存

收缩
1. 当负载因子小于0.1时，程序对哈希表进行收缩操作

### 渐进式rehash
rehash并不是由一次性，集中式完成的，而是分多次，渐进式地完成的
步骤：
1. 为ht[1]分配空间
2. 在字典中维持一个索引计数器变量rehashidx，并设置为0,表示rehash正式开始
3. rehash期间，每次对字典执行添加，删除，查找，更新操作时，程序除了执行指定操作外，也会同时将ht[0]哈希表在rehashidx索引上的所有键值对rehash到ht[1]（避免集中式rehash带来庞大的计算量），当rehash完成，rehashidx属性值增加一
4. 最终在某个时间点上，ht[0]上的所有键值对都被rehash到ht[1]上，这时程序将rehashidx设置为-1，表示rehash操作已经完成

**本文来自redis设计与实现**
当有新的键值要插入字典时，程序会对键进行hash值计算，然后计算其索引值，其步骤如下
* 计算hash：hash=dict->type->hashFunction(key);
* 计算索引：indix=hash & dict->ht[x].sizemask;  
redis使用的是MurmurHash2来计算键的哈希值

### 解决键的冲突
redis采用链地址法来解决键的冲突，即每个哈希表节点有一个next指针，多个哈希表节点可以使用next指针构成一个单链表，被分配到同一个索引的多个节点就可以使用这个单链表连接起来，以解决键的冲突。

### rehash（重新散列）
rehash是对哈希表进行相应的扩展和收缩，即哈希表重新分配内存的过程，以维持哈希表的负载因子在一个合理的范围
redis对字典的哈希表rehash步骤如下
1. 为字典的ht[1]哈希表分配空间，其分配的大小取决于执行的操作以及ht[0]当前包含的键值对的数量（ht[0].used的值）
> * 当是扩展操作，则ht[1]的大小为第一个大于等于ht[0].used*2的2^n（2的n次幂）
> * 当执行的时收缩操作，则ht[1]的大小为第一个大于等于ht[0].used的2^n
2. 将保存在ht[0]中的所有键值对rehash到ht[1]上面
3. 当ht[0]包含的所有键值对迁移到ht[1]之后（ht[0]变为空表），释放ht[0],将ht[1]设置为ht[0]，并在ht[1]新创键一个空哈希表，为下次rehash做准备

### 哈希表的负载因子
负载因子=哈希表已保存节点数量/哈希表大小  
load_factor=ht[0].used/ht[0].size

### 哈希表的扩展与收缩
扩展  
1. 当服务器目前没有执行BGSAVE或者BGREWRITEAOF命令，且哈希表的负载因子大于等于1
2. 服务器目前正在执行BGSAVE或者BGREWRITEAOF命令,并且哈希表的负载因子大于等于5
* 在执行BGSAVE或者BGREWRITEAOF命令过程中，redis需要创建当前服务器进程的子进程，而大多数操作系统采用写时复制技术来优化子进程的使用效率，所以在子进程存在期间，服务器会提高执行扩展操作所需要的负载因子，从而避免在子进程存在期间进行哈希表扩展的操作，这可以避免不必要的内存写入，最大限度节约内存

收缩
1. 当负载因子小于0.1时，程序对哈希表进行收缩操作

### 渐进式rehash
rehash并不是由一次性，集中式完成的，而是分多次，渐进式地完成的
步骤：
1. 为ht[1]分配空间
2. 在字典中维持一个索引计数器变量rehashidx，并设置为0,表示rehash正式开始
3. rehash期间，每次对字典执行添加，删除，查找，更新操作时，程序除了执行指定操作外，也会同时将ht[0]哈希表在rehashidx索引上的所有键值对rehash到ht[1]（避免集中式rehash带来庞大的计算量），当rehash完成，rehashidx属性值增加一
4. 最终在某个时间点上，ht[0]上的所有键值对都被rehash到ht[1]上，这时程序将rehashidx设置为-1，表示rehash操作已经完成

**本文来自redis设计与实现**