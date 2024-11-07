## 多进程编程

### fork系统调用
```c
#include <sys/types.h>
#include <unistd.h>
pid_t fork(void);
```
- 该函数的每次调用都返回两次，在父进程中返回的是子进程的PID，在子进程中则返回0。
- 该返回值是后续代码判断当前进程是父进程还是子进程的依据。
- fork调用失败时返回-1，并设置 errno。

fork函数复制当前进程，在内核进程表中创建一个新的进程表项。新的进程表项有很多属性和原进程相同，比如堆指针、栈指针和标志寄存器的值。但也有许多属性被赋予了新的
值，比如该进程的PPID被设置成原进程的PID，信号位图被清除(原进程设置的信号处理函数不再对新进程起作用)。
子进程的代码与父进程完全相同，同时它还会复制父进程的数据(堆数据、栈数据和静态数据)。数据的复制采用的是所谓的写时复制(copyonwrite)，即只有在任一进程(父进
程或子进程)对数据执行了写操作时，复制才会发生(先是缺页中断，然后操作系统给子进程分配内存并复制父进程的数据)。即便如此，如果我们在程序中分配了大量内存，那么使
用 fork 时也应当十分谨慎，尽量避免没必要的内存分配和数据复制。

### exec系统调用
有时我们需要在子进程中执行其他程序，即换当前进程映像，这就需要使用exec
```c
#include <unistd.h>
extern char** environ;
int execl(const char* path, const char* arg, ...);
int execlp(const char* file, const char* arg, ...);
int execle(const char* path, const char* arg, ..., char* const envp[]);
int execv(const char* path, char* const argv[]);
int execvp(const char* file, char* const argv[]);
int execve(const char* path, char* const argv[], char* const envp[]);
```
- path: 指定可执行文件的完整路径
- file: 可以接受文件名 该文件的具体位置则在环境变量PATH中搜寻 
- arg: 接受可变参数
- argv: 接受参数数组 他们都会被传给新的main函数
- envp: 用于设置新程序的环境变量


### 处理僵尸进程
对于多进程程序而言，父进程一般需要跟踪子进程的退出状态。因此，当子进程结束运行时，内核不会立即释放该进程的进程表表项，以满足父进程后续对该子进程退出信息的查
询(如果父进程还在运行)。在子进程结束运行之后，父进程读取其退出状态之前，我们称该子进程处于僵尸态。另外一种使子进程进人尸态的情况是:父进程结束或者异常终止，而
子进程继续运行。此时子进程的PPID将被操作系统设置为1，即init进程。init进程接管了该子进程，并等待它结束。在父进程退出之后，子进程退出之前，该子进程处于尸态。
```c
#include <sys/types.h>
#include <sys/wait.h>
pid_t wait(int* stat_loc);
pid_t waitpid(pid_t pid, int* stat_loc, int options);
```

### 管道
管道能在父、子进程间传递数据，利用的是fork调用之后两个管道文件描述符(d[0]和 fd[1])都保持打开。一对这样的文件描述符只能保证父、子进程间一个方向的数据传输，
父进程和子进程必须有一个关闭fd[0]，另一个关闭f[1]

### 信号量
- [ ] 待更新

### 共享内存
- [ ] 待更新

### 消息队列
- [ ] 待更新

### IPC命令
- [ ] 待更新

### 进程间传递文件描述符
- [ ] 待更新