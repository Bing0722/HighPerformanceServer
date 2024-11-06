## 高级I/O函数

### pipe函数
用于创建管道 以实现进程间的通信 
```c
#include <unisted.h>
int pipe(int fd[2]);
```
- `pipe`函数的参数是一个包含两个`int`型整数的数组指针 
- 成功时返回 `0` 并将一对打开的文件描述符值填入其参数指向的数组
- 失败返回 `-1`并设置`errno`

创建双向管道
```c
#include <sys/types.h>
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int fd[2];)
```
- 前三个参数的含义与`socket`的系统调用的三个参数完全相同 
- 最后一个参数与`pipe`系统调用的参数一样 只不过`socketpair`创建的文件描述符即可读又可写
- 成功时返回 `0` 失败返回 `-1`并设置`errno`

### dup函数和dup2函数
标准输入重定向到一个文件 或者吧标准输入重定向到一个网络连接
```c
#include <unisted.h>
int dup(int file_descriptor);
int dup2(int file_descriptor_one, int file_descriptor_two);
```
- 用于创建一个新的文件描述符 该文件描述符和原有的文件描述符`file_descriptor`指向相同的文件、管道或者网络连接
- `dup`返回的文件描述符总是取系统当前可用的最小整数值
- `dup2`和`dup`类似 不过返回第一高峰不小于`file_descriptor_two`的整数值
- 两个系统调用失败时返回 `-1`并设置`errno`
> 通过`dup`和`dup2`创建的文件描述符并不继承原文件描述符的属性 比如`close-on-exec`和`non-blocking`等

### readv函数和writev函数
`readv`函数将数据从文件描述符读到分散的内存块中 即分散读 
`writev`函数则将多块分散的内存数据一并写入文件描述符中 即集中写
```c
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovc* vector, int count);
ssize_t writev(int fd, const strcut iovc* vector, int count);
```
- `fd`: 被操作的目标文件描述符
- `vector`: `ivoec`结构数组
- `count`: 是`vector`数组的长度 即有多少内存数据需要从`fd`读出或写到`fd`
- 两个函数 成功时返回读出/写入`fd`的字节数 失败则返回 `-1`并设置`errno`

### sendfile函数
`sendfile`函数在两个文件描述符之间直接传递数据(完全在内核中操作)
从而避免了内核缓冲区和用户缓冲区之间的数据拷贝 效率很高 这杯称为零拷贝
```c
#include <sys/sendfile.h>
ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);
```
- `in_fd`: 是待读出内容的文件描述符
- `out_fd`: 是待写入内容的文件描述符
- `offset`: 指定在文件描述符的那个位置开始读 如果为空则使用读入文件流默认的起始位置
- `count`: 指定在文件描述符`in_fd`和`out_fd`之间传输的字节数
- 成功时返回传输的字节数 失败返回 `-1`并设置`errno`

> `in_fd`必须是一个支持类似`mmap`函数的文件描述符 即他必须指向真实的文件不能是`socket`和管道

> 而`out_fd`则必须是`socket`

### mmap函数和munmap函数
`mmap`函数用于申请一段内存空间 我们可以将这段内存作为进程间通信的共享内存 也可以将文件直接映射到其中
`munmap`函数则是释放由`mmap`创建的这段内存空间
```c
#include <sys/mman.h>
void* mmap(void* start, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void* start, size_t length);
```
- `start`: 运行用户使用某个特定的地址作为这段内存的起始地址 如果为`NULL`则系统自动分配一个地址
- `length`: 指定内存段的长度 
- `prot`: 用来设置内存段的访问权限
    - `PROT_READ`: 内存段可读 
    - `PROT_WRITE`: 内存段可写
    - `PROT_EXEC`: 内存段可执行
    - `PROT_NONE`: 内存段不能被访问
- `flags`: 控制内存段内容被修改后程序的行为
- `fd`: 是被映射文件对应的文件描述符 一般通过`open`的系统调用获得
- `offset`: 设置文件的何处开始映射
- `mmap`函数成功时返回指向目标内存区域的指针 失败返回`MAP_FAILED((void*)-1)`并设置`errno`
- munmap函数成功时返回 `0` 失败则返回 `-1`并设置`errno`

|常用值|含义|
|-----|----|
|MAP_SHARED|在进程间共享这段内存 对该内存的修改将反映带被映射到文件中 他提供了进程间共享内存的POSIX方法|
|MAP_PRIVATE|内存段为调用进程私有 对该段内存段的修改不会反映带被映射的文件中|
|MAP_ANONYMOUS|这段内存不是从文件映射而来的 其内容被初始化为全0 这种情况下 mmap函数的最后两个参数将被忽略|
|MAP_FIXED|内存段必须位于start参数指定的地址处 start必须是内存页面大小(4096字节)的整数倍|
|MAP_HUGETLB|按照“大内存页面”来分配内存空间 “大内存页面” 的大小可以通过/proc/mcminfo文件来查看|

### splice函数
`splice`函数用于在两个文件描述符之间移动数据 也是零拷贝操作
```c
#include <fcntl.h>
ssize_t splice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);
```
- `fd_in`: 是待传入数据的文件描述符
- 如果`fd_in`是管道文件描述符 那么`off_in`参数必须设置为`NULL` 
- 如果`fd_in`不是管道文件描述符(比如`socket`) 那么`off_in`表述从输入数据流的何处开始读取数据 此时若`off_in`被设置为`NULL` 则表示输入数据流的当前偏移位置可读入 若`off_in`不为`NULL` 则他将指出具体的偏移位置 
- `fd_out/off_out`: 含义与`fd_in/off_in`相同 不过用于输出数据流
- `len`: 指定移动数据的长度
- `flags`: 则控制数据如何移动

flags: ...

> 使用splice函数时 fd_in和fd_out必须至少有一个是管道文件描述符

### tee函数
`tee`函数在两个管道文件描述符之间复制数据 也是零拷贝操作 不消耗数据因此源文件描述符上的数据仍然可以用于后续的读操作
```c
#include <fcntl.h>
ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags);
```

### fcntl函数
`fcntl`函数(file control) 提供了对文件描述符的各种控制操作
```c
#include <fcntl.h>
int fcntl(int fd, int cmd, ...);
```
- `fd`: 是被操作的文件描述符
- `cmd`: 指定执行何种类型的操作
