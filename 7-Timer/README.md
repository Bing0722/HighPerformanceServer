## 定时器

### socket选项SO_RCVTIMEO和SO_SNDTIMEO
`socket`选项`SO_RCVTIMEO`和`SO_SNDTIMEO`，它们分别用来设置`socket`接收数据超时时间和发送数据超时时间。因此，这两个选项仅对与数据接收和发送相关的`socket`专用系统调用有效，这些系统调用包括`send`、`sendmsg`、`recv`、`recvmsg`、`accept`和`connect`。

|系统调用|有效选项|系统调用超时后的行为|
|-------|------|-----------------|
|send|SO_SNDTIMEO|返回-1 设置errno为EAGAIN或EWOULDBLOCK|
|sendmsg|SO_SNDTIMEO|返回-1 设置errno为EAGAIN或EWOULDBLOCK|
|recv|SO_RCVTIMEO|返回-1 设置errno为EAGAIN或EWOULDBLOCK|
|recvmsg|SO_RCVTIMEO|返回-1 设置errno为EAGAIN或EWOULDBLOCK|
|accept|SO_RCVTIMEO|返回-1 设置errno为EAGAIN或EWOULDBLOCK|
|connect|SO_SNDTIMEO|返回-1 设置errno为EINPROGRESS|

### SIGALRM信号
- [ ] 待更新

### I/O复用系统调用的超时参数
Linux下的3组 IO 复用系统调用都带有超时参数，因此它们不仅能统一处理信号和 I/O事件，也能统一处理定时事件。但是由于IO复用系统调用可能在超时时间到期之前就返回(有 I/O 事件发生)，所以如果我们要利用它们来定时，就需要不断更新定时参数以反映剩余的时间

### 高性能定时器
#### 时间轮
基于排序链表的定时器使用唯一的一条链表来管理所有定时器，所以插入操作的效率随着定时器数目的增多而降低。而时间轮使用哈希表的思想，将定时器散列到不同的链表上。这样每条链表上的定时器数目都将明显少于原来的排序链表上的定时器数目，插入操作的效率基本不受定时器数目的影响。
很显然，对时间轮而言，要提高定时精度，就要使`si`值足够小:要提高执行效率，则要求`N`值足够大。

#### 时间堆
时间堆（`Time Heap`） 是一种结合了时间和堆（`Heap`）数据结构的算法，通常用于处理定时任务调度。它主要通过堆结构来实现高效的定时任务管理，特别适用于那些任务的触发时间是动态变化的情形。