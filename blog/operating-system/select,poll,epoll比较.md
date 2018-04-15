
# I/O模式
* 阻塞I/O(Blocking IO)
* 非阻塞I/O(noblocking IO)
* I/O多路复用(IO multiplexing)
* 信号驱动I/O（signal driven IO）
* 异步I/O（asynchronous IO）

## 阻塞I/O(Blocking IO)
linux下的socket默认是阻塞IO，如下图
![阻塞I/O](../../image/operating-system/operating-system01.png)
当用户进程调用recvfrom这个系统调用，kernel就开始了IO的第一个阶段：准备数据。这个过程需要等待，也就是说数据被拷贝到操作系统内核的缓冲区中是需要一个过程的。而在用户进程这边，整个进程会被阻塞。当kernel一直等到数据准备好了，它就会将数据从kernel中拷贝到用户内存，然后kernel返回结果，用户进程才解锁block状态，重新运行起来。
* 所以，阻塞I/O的特点就是在IO执行的两个阶段都被Block了

## 非阻塞I/O(noblocking IO)
linux下，可以通过设置socket使其变为non-blocking。当对一个non-blocking socket执行读操作时，流程是这样
![非阻塞I/O](../../image/operating-system/operating-system02.png)   
当用户发出read操作时，如果kernel中的数据还没准备好，那么它病不会block用户进程，而是返回error。对于用户进程来说，它发起一个read操作后，并不需要等待，而是马上得到结果。用户进程判断结果是一个error时，就知道数据还没准备好，于是它可以再次发送read操作。一旦kernel中的数据准备好了，并且又再次收到了用户进程的system call，那么它马上就将数据拷贝到了用户内存，然后返回。
* 非阻塞I/O的特点是用户进程需要不断主动询问kernel数据准备好了没有

## I/O多路复用(IO multiplexing)
就是我们所说的select，poll，epoll。其好处就是单个process可以同时处理多个网络连接的IO。它的基本原理就是select，poll，epoll这些function会不断轮训所负责的所有socket，当某个socket所有数据到达了，就通知用户进程。
![I/O多路复用](../../image/operating-system/operating-system03.png)  
当用户进程调用了select，那么整个进程就会被block，同时，kernel会“监视”所有select负责的socket，当任何一个soclet中的数据准备好了，select就会返回。这个时候用户进程再调用read操作，将数据从kernel拷贝到用于进程。
* I/O多路复用的特点是通过一种机制一个进程能同时等待多个文件描述符，而这些文件描述符（套接字描述符）其中的任何一个进入读就绪状态，select()函数就可以返回。
* 如果处理的连接数不是很高的话，使用select/epoll的web server不一定比使用多线程+阻塞I/O的web server性能好，可能延迟还更大。select/epoll的优势并不是对单个连接能处理得更快，而是在于能处理更多的连接。
* 实际中，对于每个socket，一般都设置成为non-blocking，不过整个process是被select这个函数block，而不是被socket IO给block。


## 异步I/O(asynchronous IO)
linux下使用asynchronous IO其实使用得很少
![异步I/O](../../image/operating-system/operating-system04.png)
当用户进程发起read操作后，立刻就可以开始去做其他的事。而另一方面，从kernel的角度，当它收到一个asynchronous read后，它会立刻返回，所以不会对用户进程产生任何block。然后，kernel会等待数据准备完成，然后将数据拷贝到用户内存，当这一切都完成之后，kernel会给用户进程发送一个signal，告诉它read操作完成了。

* 阻塞I/O，非阻塞I/O，I/O多路复用属于同步IO

参考
[Linux IO模式及 select、poll、epoll详解](https://segmentfault.com/a/1190000003063859)