## 高性能服务器程序框架

### 服务器模型
#### C/S模型
C/S模型的逻辑很简单。服务器启动后，首先创建一个(或多个)监听`socket`，并调用`bind`函数将其绑定到服务器感兴趣的端口上，然后调用`listcn`函数等待客户连接。服务器稳定运行之后，客户端就可以调用`connect`函数向服务器发起连接了

> C/S模型非常适合资源相对集中的场合，并且它的实现也很简单，但其缺点也很明显:服务器是通信的中心，当访问量过大时，可能所有客户都将得到很慢的响应。

#### P2P模型
P2P模型使得每台机器在消耗服务的同时也给别人提供服务，这样资源能够充分、自由地共享。云计算机群可以看作P2P模型的一个典范。但P2P模型的缺点也很明显:当用户之间传输的请求过多时，网络的负载将加重。

### 服务器编程框架
|模块|单个服务器程序|服务器机群|
|----|-----------|--------|
|I/O处理单元|处理客户连接 读取网络数据|作为接入服务器 实现负载均衡|
|逻辑单元|业务进程或线程|逻辑服务器|
|网络存储单元|本地数据库、文件或缓存|数据库服务器|
|请求队列|个单元之间的通信方式|个服务器之间的永久TCP连接|

### I/O模型
- [ ] 等待更新

### 两种高效的事件处理模式
随着网络设计模式的兴起，Reactor和Proactor事件处理模式应运而生。同步O模型通常用于实现 Reactor 模式，异步 I/0 模型则用于实现Proactor 模式。不过后面我们将看到，如何使用同步 I/O方式模拟出 Proactor 模式。

#### Reactor模式
Reactor是这样一种模式，它要求主线程(1/0处理单元，下同)只负责监听文件描述上是否有事件发生，有的话就立即将该事件通知工作线程(逻辑单元，下同)。除此之外，主线程不做任何其他实质性的工作。读写数据，接受新的连接，以及处理客户请求均在工作线程中完成。

使用同步 I/0 模型(以 `epoll_wait` 为例)实现的 Reactor 模式的工作流程是:
1. 主线程往`epoll`内核事件表中注册`socket`上的读就绪事件。
2. 主线程调用epoll_wait 等待 `socket`上有数据可读。
3. 当`socket`上有数据可读时，`epoll_wait`通知主线程。主线程则将`socket` 可读事件放入请求队列。
4. 睡眠在请求队列上的某个工作线程被唤醒，它从`socket`读取数据，并处理客户请求然后往`epoll`内核事件表中注册该`socket`上的写就绪事件。
5. 主线程调用 `epoll_wait` 等待 `socket` 可写。
6. 当`socket` 可写时，`epoll_wait`通知主线程。主线程将`socket`可写事件放入请求队列。
7. 睡眠在请求队列上的某个工作线程被唤醒，它往`socket`上写人服务器处理客户请求的结果。

#### Proactor模式
与 Reactor 模式不同，Proactor模式将所有IO操作都交给主线程和内核来处理，工作线程仅仅负责业务逻辑

使用异步 I/O 模型(以 `aio_read` 和 `aio_write` 为例)实现的 Proactor 模式的工作流程是:
1. 主线程调用`aioread`函数向内核注册`socket`上的读完成事件，并告诉内核用户读缓冲区的位置，以及读操作完成时如何通知应用程序(这里以信号为例，详情请参考`sigevent`的 `man` 手册)。
2. 主线程继续处理其他逻辑。
3. 当`socket` 上的数据被读入用户缓冲区后，内核将向应用程序发送一个信号，以通知应用程序数据已经可用。
4. 应用程序预先定义好的信号处理函数选择一个工作线程来处理客户请求。工作线程处理完客户请求之后，调用`aio_write` 函数向内核注册`socket`上的写完成事件，并告诉内核用户写缓冲区的位置，以及写操作完成时如何通知应用程序(仍然以信号为例)。
5. 主线程继续处理其他逻辑。
6. 当用户缓冲区的数据被写人`socket`之后，内核将向应用程序发送一个信号，以通知应用程序数据已经发送完毕。
7. 应用程序预先定义好的信号处理函数选择一个工作线程来做善后处理，比如决定是否关闭 `socket`。

### 两种高效的并发模式

#### 半同步/半异步模式
半同步/半异步模式中，同步线程用于处理客户逻辑，异步线程用于处理I/O事件。异步线程监听到客户请求后就将其封装成请求对象并插入请求队列中。请求队列将通知某个工作在同步模式的工作线程来读取并处理该请求对象。具体选择哪个工作线程来为新的客户请求服务，则取决于请求队列的设计。比如最简单的轮流选取工作线程的RoundRobin算法

#### 领导者/追随者模式
领导者/追随者模式是多个工作线程轮流获得事件源集合，轮流监听、分发并处理事件的一种模式。在任意时间点，程序都仅有一个领导者线程，它负责监听I0事件。而其他线程则都是追随者，它们休眠在线程池中等待成为新的领导者。当前的领导者如果检测到IO事件，首先要从线程池中推选出新的领导者线程，然后处理I/O事件。此时，新的领导者等待新的 I/O 事件，而原来的领导者则处理 I/O事件，二者实现了并发。

领导者/追随者模式包含如下几个组件:句柄集(HandleSet)、线程集(ThreadSet)、事件处理器(EventHandler)和具体的事件处理器(ConcreteEventHandler)。

1. 句柄集
句柄(Handle)用于表示O资源，在Linux下通常就是一个文件描述符。句柄集管理众多句柄，它使用wait_for_event方法来监听这些句柄上的I/O事件，并将其中的就绪事件通知给领导者线程。领导者则调用绑定到Handle上的事件处理器来处理事件。领导者将Handle 和事件处理器绑定是通过调用句柄集中的register_handle 方法实现的。

2. 线程集
这个组件是所有工作线程(包括领导者线程和追随者线程)的管理者。它负责各线程之间的同步，以及新领导者线程的推选。线程集中的线程在任一时间必处于如下三种状态之一:
    - Lcader:线程当前处于领导者身份，负责等待句柄集上的O事件。
    - Processing:线程正在处理事件。领导者检测到IO事件之后，可以转移到Processing状态来处理该事件，并调用promote_newleader方法推选新的领导者;也可以指定其他追随者来处理事件(EventHandof)，此时领导者的地位不变。当处于Processing状态的线程处理完事件之后，如果当前线程集中没有领导者，则它将成为新的领导者，否则它就直接转变为追随者。
    - Follower:线程当前处于追随者身份，通过调用线程集的join方法等待成为新的领导者，也可能被当前的领导者指定来处理新的任务。

3. 事件处理器和具体的事件处理器
事件处理器通常包含一个或多个回调函数handle_event。这些回调函数用于处理事件对应的业务逻辑。事件处理器在使用前需要被绑定到某个句柄上，当该句柄上有事件发生时，领导者就执行与之绑定的事件处理器中的回调函数。具体的事件处理器是事件处理器的派生类。它们必须重新实现基类的handle_event方法，以处理特定的任务。
### 有限状态机

- [ ] 待更新

### 提高服务器性能的其他建议
性能对服务器来说是至关重要的，毕竟每个客户都期望其请求能很快地得到响应。影响
服务器性能的首要因素就是系统的硬件资源，比如CPU的个数、速度，内存的大小等。不过由于硬件技术的飞速发展，现代服务器都不缺乏硬件资源。因此，我们需要考虑的主要问题是如何从“软环境”来提升服务器的性能。服务器的“软环境”，一方面是指系统的软件资源，比如操作系统允许用户打开的最大文件描述符数量;另一方面指的就是服务器程序本身，即如何从编程的角度来确保服务器的性能

#### 池
既然服务器的硬件资源“充裕”，那么提高服务器性能的一个很直接的方法就是以空间
换时间，即“浪费”服务器的硬件资源，以换取其运行效率。这就是池(poo1)的概念。池是一组资源的集合，这组资源在服务器启动之初就被完全创建好并初始化，这称为静态资源分配。当服务器进入正式运行阶段，即开始处理客户请求的时候，如果它需要相关的资源，就可以直接从池中获取，无须动态分配。很显然，直接从池中取得所需资源比动态分配资源的速度要快得多，因为分配系统资源的系统调用都是很耗时的。当服务器处理完一个客户连接后，可以把相关的资源放回池中，无须执行系统调用来释放资源。从最终的效果来看，池相当于服务器管理系统资源的应用层设施，它避免了服务器对内核的频繁访问。

#### 数据复制
高性能服务器应该避免不必要的数据复制，尤其是当数据复制发生在用户代码和内核之
间的时候。如果内核可以直接处理从socket 或者文件读人的数据，则应用程序就没必要将这些数据从内核缓冲区复制到应用程序缓冲区中。这里说的“直接处理”指的是应用程序不关心这些数据的内容，不需要对它们做任何分析。比如p服务器，当客户请求一个文件时，服务器只需要检测目标文件是否存在，以及客户是否有读取它的权限，而绝对不会关心文件的具体内容。这样的话，ûp服务器就无须把目标文件的内容完整地读入到应用程序缓冲区中并调用send 函数来发送，而是可以使用“零拷贝”函数sendfle 来直接将其发送给客户端。

#### 上下文切换
并发程序必须考虑上下文切换(contextswitch)的问题，即进程切换或线程切换导致的的系统开销。即使是IO密集型的服务器，也不应该使用过多的工作线程(或工作进程，下同)，否则线程间的切换将占用大量的CPU时间，服务器真正用于处理业务逻辑的CPU时间的比重就显得不足了。因此，为每个客户连接都创建一个工作线程的服务器模型是不可取的。半同步/半异步模式是一种比较合理的解决方案，它允许一个线程同时处理多个客户连接。此外，多线程服务器的一个优点是不同的线程可以同时运行在不同的CPU上。当线程的数量不大于CPU的数目时，上下文的切换就不是问题了。