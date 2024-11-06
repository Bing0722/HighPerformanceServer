## Linux服务器程序的规范

### 日志
#### syslog函数
应用程序使用`syslog`函数与`rsyslogd`守护进程通信
```c
#include <syslog.h>
void syslog(int priority, const char* message, ...);
```
改变`syslog`的默认输出方式
```c
#include <syslog.h>
void openlog(const char* ident, int logopt, int facility);
```
设置`syslog`的日志掩码
```c
#include <syslog.h>
int setlogmask(int maskpri);
```
关闭日志功能
```c
#include <syslog.h>
void closelog();
```

### 用户信息
#### UID、EUID、GID、EGID
真实用户ID(UID) 有效用户ID(EUID) 真实组ID(GID) 有效组ID(EGID)
```c
#include <sys/types.h>
#include <unisted.h>
uid_t getuid();         /* 获取真实用户ID */
uid_t geteuid();        /* 获取有效用户ID */
gid_t getgid();         /* 获取真实组ID */
gid_t getegid();        /* 获取有效组ID */
int setuid(uid_t uid);  /* 设置真实用户ID */
int seteuid(uid_t uid); /* 设置有效用户ID */
int setgid(gid_t gid):  /* 设置真实组ID */
int setegid(gid_t gid); /* 设置有效组ID */
```

### 进程间关系
#### 进程组
Linux下每个进程都隶属于一个进程组，因此它们除了PID信息外，还有进程组ID
(PGID)。我们可以用如下函数来获取指定进程的PGID:
```c
#include <unistd.h>
pid_t getpgid(pid_t pid);
```

下面的函数用于设置 PGID:
```c
#include <unistd.h>
int setpgid(pid_t pid, pid_t pgid);
```

#### 会话
一些有关联的进程组将形成一个会话(session)。下面的函数用于创建一个会话:
```c
#include <unistd.h>
pid_t setsid(void);
```
读取会话
```c
#include <unistd.h>
pid_t getsid(pid_t pid);
```

### 系统资源限制
Linux上运行的程序都会受到资源限制的影响，比如物理设备限制(CPU数量、内存
数量等)、系统策略限制(CPU时间等)，以及具体实现的限制(比如文件名的最大长度)。
Linux系统资源限制可以通过如下一对函数来读取和设置:
```c
#include <sys/resource.h>
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const strcut rlimit * rlim);

struct rlimit{
    rlimi_t rlim_cur;
    rlimi_t rlim_max;
}
```

### 改变工作目录和根目录
获取进程当前工作目录和改变进程工作目录的函数分别是
```c
#include <unistd.h>
char* getcwd(char* buf, size_t size);
int chdir(const char* path);
```
- `buf`: 指向的内存用于存储进程当前工作目录的绝对路径名，其大小由`size`参数指定
- `chdir`: 函数的 `path`参数指定要切换到的目标目录。它成功时返回`0`，失败时返回`-1`并设
置 `errno`。