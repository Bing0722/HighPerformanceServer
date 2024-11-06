## 信号

### Linux信号概述
#### 发送信号
Linux下，一个进程给其他进程发送信号的API是`kill` 函数。其定义如下
```c
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
```
- 该函数把信号`sig`发送给目标进程
- 函数成功时返回 `0` 失败返回 `-1`并设置`errno`

#### 信号处理方式
... 

#### Linux信号
|信号|起源|默认行为|含义|
|---|----|------|---|
|SIGINT|ANSI|Term|键盘输入以中断进程(Ctrl+C)|
|SIGHUP|POSIX|Term|控制终端挂起|
|SIGPIPE|POSIX|Term|往读端被关闭的管道或者socket连接中写入数据|
|SIGURG|4.2BSD|lgn|socket连接上收到紧急数据|
|SIGALRM|POSIX|Term|由alarm或setitimer设置的实时闹钟超时引起|
|SIGCHLD|POSIX|lgn|子进程状态发送变化(退出或者暂停)

> 太多了 不写了

### 信号函数
#### signal系统调用
要为一个信号设置处理函数，可以使用下面的`signal`系统调用:
```c
#include <signal.h>
int sigaction(int sig, const struct sigaction* act, struct sigaction* oact);
```
- sig: 指出要捕获的信号类型
- act: 指定新的信号处理方式
- oact: 输出信号先前的处理方式

### 信号集
- [ ] 待更新

### 统一事件源
- [ ] 待更新

### 网络编程相关的信号
- [ ] 待更新
