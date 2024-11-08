## 服务器调试、调试与测试

### 最大文件描述符数
文件描述符是服务器程序的宝贵资源，几乎所有的系统调用都是和文件描述符打交道系统分配给应用程序的文件描述符数量是有限制的，所以我们必须总是关闭那些已经不再使用的文件描述符，以释放它们占用的资源。比如作为守护进程运行的服务器程序就应该总是关闭标准输人、标准输出和标准错误这3个文件描述符。

Linux 对应用程序能打开的最大文件描述符数量有两个层次的限制:用户级限制和系统级限制。用户级限制是指目标用户运行的所有进程总共能打开的文件描述符数:系统级的限制是指所有用户总共能打开的文件描述符数

### 调整内核参数
几乎所有的内核模块，包括内核核心模块和驱动程序，都在/proc/sys 文件系统下提供了某些配置文件以供用户调整模块的属性和行为。通常一个配置文件对应一个内核参数，文件名就是参数的名字，文件的内容是参数的值。我们可以通过命令sysctl -a查看所有这些内核参数

###  GDB调试
- [ ] 待更新

### 压力测试
压力测试程序有很多种实现方式，比如I/0复用方式，多线程、多进程并发编程方式以及这些方式的结合使用。不过，单纯的I/0复用方式的施压程度是最高的，因为线程和进程的调度本身也是要占用一定CPU时间的。因此，我们将使用epoll 来实现一个通用的服务器压力测试程序