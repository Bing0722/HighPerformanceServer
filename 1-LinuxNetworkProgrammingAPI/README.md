## Linux网络编程基础API

### 字节转换
```c
#include <netinet/in.h>
htonl: unsigned long int    host to network long
htons: unsigned short int   host to network short
ntohl: unsigned long int    network to host long
ntohs: unsigned short int   network to host short
```

### IP地址转换
```c
#include <arpa/inet.h>
in_addr_t inet_addr(const char* strptr);
int inet_aton(const char* cp, struct ip_addr* inp);
char* inet_ntoa(struct in_addr in);

#include <netinet/in.h>
int inet_pton(int af, const char* src, void* dst);
const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt);
#define INET_ADDRESTRLEN 16
#define INET6_ADDRESTRLEN 46
```
- `inet_addr`: 将点分十进制转换成网络字节序
- `inet_aton`: 功能同 `inet_addr`
- `inet_ntoa`: 将网络字节序转换成点分十进制
- `inet_pton`: 将字符串表示的IP地址`src`(点分十进制)转换成网络字节序IP地址存储于`dst`
    - `af`: 指定地址族
- `inet_ntop`: 与`inet_pton`相反
- `INET_ADDRESTRLEN`和`INET6_ADDRESTRLEN`: 是cnt指定目标存储单元的大小的宏

### 创建socket
```c
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```
- `domain`: 使用那个底层协议
- `type`: 指定服务类型 
    - TCP流服务(SOCK_STREAM)
    - UDP数据报(SOCK_UGRAM)
- `protocol`: 基本为 `0`
- 成功返回一个`socket`文件描述符 失败返回 `-1` 并设置`errno`

### 命名socket
```c
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen);
```
- `bind` 将 `my_addr` 所指的`socket`地址分配给未命名的 `sockfd` 文件描述符 
- `addrlen` 参数为 `socket` 的地址长度
- 成功返回 `0` 失败返回 `-1` 并设置 `errno` 常见`errno`有 `EACCES` `EADDEINUSE`
    - `EACCES`: 被绑定的地址是受保护的地址 仅超级用户能访问
    - `EADDRINUSE`: 被绑定的地址正在使用

### 监听socket
```c
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```
- `sockfd`: 指定被监听的`socket`
- `backlog`: 提示内核监听队列的最大长度 如果超过则不受理新的客户连接 

### 接受连接
```c
#include <sys/types.h>
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- `sockfd`: 是执行过`listen`系统调用的监听`socket` 
- `addr`: 是用来获取被接受连接的远端`socket`地址 该地址的长度由`addrlen`参数指出
- `accept`: 成功时返回一个新的连接`socket` 
- 该连接唯一地标识了被接受的这个连接 服务器可以通过读写该`socket` 来与被连接的客户端通信
- 成功返回一个新的`socket` 失败返回 `-1`并`errno`

### 发起连接
```c
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *serve_addr, socklen_t addrlen);
```
- `sockfd`: 由`socket`系统调用返回一个`socket`
- `serv_addr`: 是服务器监听的`socket`地址
- `addrlen`: 则是指定这个地址的长度
- 成功返回 `0` 失败返回 `-1`并设置`errno` 其中两种常见的`errno` `ECONNREFUSED` 和 `ETIMEDOUT`
- `ECONNREFUSED`: 目标端口不存在 连接被拒绝
- `ETIMEDOUT`: 连接超时

### 关闭连接
```c
#include <unisted.h>
int close(int fd);

#include <sys/socket.h>
int shutdown(int sockfd, int howto);
```
- `fd`: 是待关闭的`socket` 不过 `close`系统调用并非总是立即关闭一个连接 而是将`fd`的引用次数减`1`
- 只有当`fd`的引用次数为`0`时 才是真正关闭连接
- `sockfd`: 是待关闭的`socket` `howto`参数决定了`shutdowm`的行为
- `SHUT_RD`: 关闭`sockfd`上读的这一半 `socket`不能进行读操作 并且接受缓冲区中的数据都被丢弃
- `SHUT_WR`: 关闭`sockfd`上写的这一半 发送缓冲区中的数据在真正关闭前全部发送出去 不能再进行写操作 处于半关闭
- `SHUT_RDWR`: 同时关闭`sockfd`上的读写
- 成功时返回`0` 失败则返回 `-1`并设置`errno`

### TCP数据读写
```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t len, int flags)
ssize_t send(int sockfd, const void* buf, size_t len, int flags)
```
- `recv`读取`sockfd`上的数据 `buf`和`len`参数分别指定读缓冲区的位置和大小 `flags` 可选值 一般为`0`
- `recv`成功时返回实际读取到的数据的长度 它可能小于`len`
- 如果`recv`返回`0` 表示对方关闭了连接 如果出错返回 `-1`并设置`errno`
- `send`和`recv`用法基本相同


#### flags参数

|选项名|含义|send|recv|
|-----|---|----|----|
|MSG_CONFIRM|指示数据链路层协议持续监听对方的回应 知道得到答复 仅适用于SOCK_DGRAM和 SOCK_RAW 类型的socket|Y|N|
|MSG_DONTROUTE|不查看路由表 直接将数据发送给本地局域网内的主机 表示发送者确切知道目标主机就在本地网络上 |Y|N|
|MSG_DONTWAIT|对socket的此次操作将是非阻塞的|Y|Y|
|MSG_MORE|告诉内核还有更多数据要发送 内核将超时等待新数据写入tcp发送缓冲区后一并发送 防止发送过小 提高速率|Y|N|
|MSG_WAITALL|读操作仅在读取到指定数量的字节后才返回|N|Y|
|MSG_PEEK|窥探读缓存中的数据 此处读操作不会导致这些数据被清除|N|Y|
|MSG_OOB|发送或接受紧急数据|Y|Y|
|MSG_NOSIGNAL|往读端关闭的管道或者socket连接中写数据时不引发SIGPIPE信号|Y|N|


### UDP数据读写
```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recvform(int sockfd, void *buf size_t len, int flags, struct sockaddr* src_addr, socklen_t addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr* des_addr, socklen_t addrlen);
```
- `recvfrom`读取`sockfd`上的数据 `buf`和`len`参数分别指定缓冲区的位置和大小
- 因为UDP通信没有连接的概念 所以每次都需要获取发送端的端口 的地址`src_addr`和地址的长度`addrlen`
- `send`和`recvform`用法基本相同
- `flags`参数及返回值的含义均与`send/recv`系统调用的`flags`参数和返回值相同

### 通用数据的读写
```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags);
ssize_t sendmsg(int sockfd, struct msghdr* msg, int flasg);
```
- `sockfd`参数指定被操作的目标`socket` `msg`参数是`msghdr`结构体类型的指针 `flags` 参数及返回值同`send/recv`

### 带外标记
```c
#include <sys/socket.h>
int sockatmark(int sockfd);
```
- `sockatmark`判断`sockfd`是否处于外带标记 即下一个被读取到的数据释放是带外数据
- 如果是 `sockatmark`返回 `1` 此时我们可以利用带`MSG_OOB`标志的`recv`调用来接受带外数据
- 如果不是 则`sockatmark`返回 `0`

### 地址信息函数
```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr* address, socklen_t* address_len);
int getpeername(int sockfd, struct sockaddr* address, socklen_t* address_len);
```
- `getsockname`: 获取`sockfd`对应的本端`socket`地址 并将其存储用户`address`中 地址长度存储`address_len`中
- 如果实际`socket`地址长度大于`address`所指内存区大小 那么`socket`地址将会截断
- `getsockname`成功时返回 `0` 失败时返回 `-1`并设置`errno`
- `getpeername`: 获取`sockfd`对应的远端`socket`地址 其参数及返回的含义与`getsockname`相同

### socket选项
```c
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int option_name, void* option_value, socklen_t* restrict option_len);
int setsockopt(int sockfd, int level, int option_name, const void* option_value, socklen_t option_len);
```
- `sockfd`: 指定被操作的目标`socket`
- `level`: 指定要操作那个协议的选项
- `option_name`: 指定选项的名字
- `option_value`和`option_len`: 分别是操作选项的值和长度
- 成功时返回`0` 失败时返回 `-1`并设置`errno`

|level|option_name|数据类型|说明|
|-----|-----------|-------|---|
|SOL_SOCKET|SO_DEBUG|int|打开调试信息|
|SOL_SOCKET|SO_REUSEADDR|int|重用本地地址|
|SOL_SOCKET|SO_TYPE|int|获取socket类型|
|SOL_SOCKET|SO_ERROR|int|获取并清除socket错误状态|
|SOL_SOCKET|SO_DONTROUTE|int|含义与send系统调用的MSG_DONTROUTE标志类似|
|SOL_SOCKET|SO_RECVBUF|int|TCP接收缓冲区大小|
|SOL_SOCKET|SO_SNDBUF|int|TCP发送缓冲区大小|
|SOL_SOCKET|SO_KEEPALIVE|int|发送周期保活报文以维持连接|
|SOL_SOCKET|SO_OOBLNLINE|int| |
|SOL_SOCKET|SO_LINGER|linger|若有数据待发送则延迟关闭|
|SOL_SOCKET|SO_RCVLOWAT|int|TCP接收缓冲区低水位标记|
|SOL_SOCKET|SO_SNDLOWAT|int|TCP发送缓冲区低水位标记|
|SOL_SOCKET|SO_RCVTIMEO|timeval|接收数据超时|
|SOL_SOCKET|SO_SNDTIMEO|timeval|发送数据超时|
|IPPROTO_IP|IP_TOS|int|服务类型|
|IPPROTO_IP|IP_TTL|int|存活时间|
|IPPROTO_IPV6|IPV6_NEXTHOP|sockaddr_in6|下一跳IP地址|
|IPPROTO_IPV6|IPV6_RECVPKTINFO|int|接收分组信息|
|IPPROTO_IPV6|IPV6_DONTFRAG|int|禁止分片|
|IPPROTO_IPV6|IPV6_RECVTCLASS|int|接收通信类型|
|IPPROTO_TCP|TCP_MAXSEG|int|TCP最大报文段大小|
|IPPROTO_TCP|TCP_NODELAY|int|禁止Nagle算法|

- 对服务器而言 socket选项要在listen系统调用之前设置才有效

#### 常用选项
- SO_REUSEADDR: 允许应用程序立即重用本地的socket地址


### 网络信息API
```c
#include <netdb.h>
struct hostent* gethostbyname(const char* name);
struct hostent* gethostbyaddr(const void* addr, size_t len, int type);
```
- `name`: 指定目标主机的主机名
- `addr`: 指定目标主机的IP地址
- `len`: 指定`addr`所指IP地址的长度
- `type`: 指定`addr`所指IP地址的类型

```c
#include <netdb.h>
struct servent* getservbyname(const char* name, const char* proto);
struct servent* getservbyport(int port, const char* proto);
```
- `name`: 指定目标服务的名字
- `port`: 指定目标服务对应的端口号
- `proto`: 指定服务类型 `tcp` 表示流服务 `udp` 表示数据报服务 `NULL`表示获取所有类型服务

#### 其他
```c
#include <netdb.h>
int getaddrinfo(const char* hostname, const char* service, const struct addrinfo* hints, struct addrinfo** result);
int getnameinfo(const struct sockaddr* sockaddr, socklen_t addrlen, char* host, socklen_t hostlen, char* serv, socklen_t servlen, int flags);
```