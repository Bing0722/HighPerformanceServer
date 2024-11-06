## I/O复用

### select系统调用
`sclcct` 系统调用的用途是:在一段指定时间内，监听用户感兴趣的文件描述符上的可读、
可写和异常等事件。本节先介绍`select`系统调用的API，然后讨论`select`判断文件描述符就
绪的条件，最后给出它在处理带外数据中的实际应用。
#### select API
```c
#include <sys/select.h>
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
```
- `nfds`: 指定被监听的文件描述符的总数 通常是`select`监听的所有文件描述符的最大值加 `1`
- `readfds`、`writefds`和`exceptfds`: 分别指向可读、可写和异常等时间对应的文件描述符集合
- `select`成功时返回就绪文件描述符的总数 如果在超时时间内没有任何文件描述符就绪 `select`返回`0`
- 失败返回 `-1`并设置`errno` 如果`select`等待期间 接收到信号 则立即返回 `-1` 并设置`errno`为`EINTR`

`fd_set`结构体中的位
```c
#include <sys/select.h>
FD_ZERO(fd_set* fdset);                 /* 清除 fdset 的所有位 */
FD_SET(int fd, fd_set* fdset);          /* 设置 fdset 的位 fd */
FD_CLR(int fd, fd_set* fdset);          /* 清除 fdset 的位 fd */
int FD_ISSET(int fd, fd_set* fdset);    /* 测试 fdset 的位 fd 是否被设置 */
```

`timeval`结构体
```c
struct timeval{
    long tv_sec;    /* 秒数 */
    long tv_usec;   /* 微秒数 */
};
```

#### 文件描述符的就绪条件
哪些情况下文件描述符可以被认为是可读、可写或者出现异常，对于`select`的使用非常关键。
在网络编程中，下列情况下`socket`可读:
1. `socket`内核接收缓存区中的字节数大于或等于其低水位标记`SO_RCVLOWAT`。此时我们可以无阻塞地读该`socket`，并且读操作返回的字节数大于`0`。
2. `socket`通信的对方关闭连接。此时对该`socket`的读操作将返回`0`。
3. 监听 `socket`上有新的连接请求。
4. `socket` 上有未处理的错误。此时我们可以使用`getsockopt` 来读取和清除该错误。

下列情况下 `socket` 可写:
1. `socket`内核发送缓存区中的可用字节数大于或等于其低水位标记`SO_SNDLOWAT`。此时我们可以无阻塞地写该 `socket`，并且写操作返回的字节数大于`0`。
2. `socket`的写操作被关闭。对写操作被关闭的`socket` 执行写操作将触发一个 `SIGPIPE` 信号。
3. `socket`使用非阻塞`connect`连接成功或者失败(超时)之后。
4. `socket`上有未处理的错误。此时我们可以使用`getsockopt`来读取和清除该错误

#### 处理带外数据
...

### poll系统调用
`poll` 系统调用和`select`类似，也是在指定时间内轮询一定数量的文件描述符，以测试其中是否有就绪者。`poll`的原型如下
```c
#include <poll.h>
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
```
- `fds`: 是一个`pollfd`结构类型的数组 它指定所有我们感兴趣的文件描述符上发生的可读、可写和异常事件
- `nfds`: 指定监听事件集合`fds`的大小
- `timeout`: 指定`poll`的超时值 单位是毫秒 `timeout `为 `-1`时 `poll`将永远阻塞直到某个事件发生 `timeout` 为`0`时 `poll`调用立即返回
- `poll`系统调用的返回值的含义与`select`相同

```c
struct pollfd{
    int fd;             /* 文件描述符 */
    short events;       /* 注册的事件 */
    short revents;      /* 实际发生的事件 由内核填充 */
}
```

### epoll系列系统调用
#### 内核事件表
`epoll`是 Linux特有的I/0复用函数。它在实现和使用上与`select`、`poll`有很大差异。首先，`epoll` 使用一组函数来完成任务，而不是单个函数。其次，`epoll`把用户关心的文件描述符上的事件放在内核里的一个事件表中，从而无须像`select`和`poll` 那样每次调用都要重复传人文件描述符集或事件集。但`epoll`需要使用一个额外的文件描述符，来唯一标识内核中的这个事件表。这个文件描述符使用如下`epoll_create` 函数来创建:
```c
#include <sys/epoll.h>
int epoll_create(int size);
```
- `size`: 现在并不起作用 只是给内核一个提示告诉它事件表需要多大
- 该函数返回的文件描述符将用作其他所有`epool`系统调用的第一个参数 以指定要访问的内核事件表

`epoll`内核事件表
```c
#include <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
```
- `fd`: 是要操作的文件描述符
- `op`: 指定操作类型
    - `EPOLL_CTL_ADD`: 往事件表中注册`fd`上的事件
    - `EPOLL_CTL_MOD`: 修改`fd`上的注册事件
    - `EPOLL_CTL_DEL`: 删除`fd`上的注册事件
- `event`: 指定事件 它是`epoll_wait`结构指针类型

### epoll_wait函数
`epoll`系列系统调用的主要接口是`epoll_wait`函数。它在一段超时时间内等待一组文件描述符上的事件，其原型如下:
```c
#include <sys/epoll.h>
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
```
- 函数成功时返回就绪的文件描述符的个数 失败返回 `-1`并设置`errno`
- `tiemout`: 与`poll`的`timeout`相同
- `maxevents`: 指定最多监听多少个事件 必须大于`0`

`epoll_wait`函数如果检测到事件，就将所有就绪的事件从内核事件表(由`epfd`参数指定)中复制到它的第二个参数`events`指向的数组中。这个数组只用于输出`epoll_wait` 检测到的就绪事件，而不像`select`和`poll`的数组参数那样既用于传入用户注册的事件，又用于输出内核检测到的就绪事件。这就极大地提高了应用程序索引就绪文件描述符的效率

#### LT(水平触发)和ET(边缘触发)模式
LT模式是默认的工作模式，这种模式下`epoll`相当于一个效率较高的`poll`。当往`epoll`内核事件表中注册一个文件描述符上的EPOLLET事件时，`epoll`将以ET模式来操作该文件描述符。ET模式是`epoll`的高效工作模式。

对于采用LT工作模式的文件描述符，当`epoll_wait`检测到其上有事件发生并将此事件通知应用程序后，应用程序可以不立即处理该事件。这样，当应用程序下一次调用`epoll_wait`时，`epoll_wait`还会再次向应用程序通告此事件，直到该事件被处理。而对于采用ET 工作模式的文件描述符，当`epoll_wait`检测到其上有事件发生并将此事件通知应用程序后，应用程序必须立即处理该事件，因为后续的`epoll_wait`调用将不再向应用程序通知这一事件。可见，ET模式在很大程度上降低了同一个`epoll`事件被重复触发的次数因此效率要比LT模式高。

#### EPOLLONESHOT事件
即使我们使用 ET模式，一个`socket`上的某个事件还是可能被触发多次。这在并发程序中就会引起一个问题。比如一个线程(或进程，下同)在读取完某个`socket`上的数据后开
始处理这些数据，而在数据的处理过程中该`socket`上又有新数据可读(`EPOLLIN`再次被触发)，此时另外一个线程被唤醒来读取这些新的数据。于是就出现了两个线程同时操作一个
`socket`的局面。这当然不是我们期望的。我们期望的是一个`socket`连接在任一时刻都只被一个线程处理。这一点可以使用`epoll`的`EPOLLONESHOT`事件实现。

对于注册了`EPOLLONESHOT`事件的文件描述符，操作系统最多触发其上注册的一个可读、可写或者异常事件，且只触发一次，除非我们使用`epolcl`函数重置该文件描述符上注
册的 `EPOLLONESHOT`事件。这样，当一个线程在处理某个`socket`时，其他线程是不可能有机会操作该 `socket`的。但反过来思考，注册了`EPOLLONESHOT`事件的`socket`一旦被某个线程处理完毕，该线程就应该立即重置这个`socket`上的 `EPOLLONESHOT`事件，以确保这个`socket`下一次可读时，其`EPOLLIN` 事件能被触发，进而让其他工作线程有机会继续处理这个 `socket`。

### 三组I/O复用函数的比较
|系统调用|select|poll|epoll|
|-------|------|----|-----|
|事件集合| 用户通过3个参数分别传入感兴趣的可读、可写及异常等事件 内核通过对这些参数的在线修改来反馈其中的就绪事件 这使得用户每次调用select都要充值这3个参数|统一处理所有事件类型 因此只需要一个事件集参数 用户通过pollfd.events传入感兴趣的事件 内核通过修改pollfd.events及反馈其中就绪的事件|内核通过一个事件表直接管理用户感兴趣的所有事件 因此每次调用epoll_wait时 无须反复传入用户感兴趣的事件 epoll_wait系统调用的参数events仅用来反馈就绪的事件 |
|应用程序索引就绪文件描述符的事件复杂度| O(n) | O(n) | O(1)|
|最大指出文件描述符| 一般有最大值限制| 65535| 65535|
|工作模式|LT|LT|支持ET高效模式|
|内核实现和工作效率|采用轮询方式检测就绪事件 时间复杂度为O(n)|采用轮询方式检测就绪事件 时间复杂度为O(n)|采用回调方式检测就绪事件 时间复杂度为O(1)|

### I/O复用的高级应用
- [ ] 待更新
### 超级服务xinetd
- [ ] 待更新 