## 多线程编程

### Linux线程概述
线程是程序中完成一个独立任务的完整执行序列，即一个可调度的实体。根据运行环境和调度者的身份，线程可分为内核线程和用户线程。内核线程，在有的系统上也称为LWP(Light Weight Process，轻量级进程)，运行在内核空间，由内核来调度;用户线程运行在用户空间，由线程库来调度。当进程的一个内核线程获得CPU的使用权时，它就加载并运行一个用户线程。可见，内核线程相当于用户线程运行的“容器"。一个进程可以拥有M个内
核线程和N个用户线程，其中M≤N。并且在一个系统的所有进程中，M和N的比值都是固定的。按照M:N的取值，线程的实现方式可分为三种模式:完全在用户空间实现、完全由内核调度和双层调度(twolevelscheduler)。

### 创建线程和结束线程
```c
#include <pthread.h>
int pthread_create(pthread_t* thread, const pthread_attr_t* attr,
    void* (*start_routine)(void*), void* arg);
```
- thread: 是新线程的标识符 
- attr: 用于设置新线程的属性 给他传NULL表示使用默认属性
- 成功返回 0 失败返回错误码

```c
#include <pthread.h>
void pthread_exit(void retval);
```
- 函数通过retval参数向线程的回收者提供其退出信息

```c
#include <pthread.h>
int pthread_join(pthread_t, void** retval);
```
- thread: 是目标线程的标识符 
- retval: 是目标线程返回的退出信息 
- 该函数会一直阻塞 知道回收的线程结束为为止
- 成功返回 0 失败返回错误码

```c
#include <pthread.h>
int pthread_cancel(pthread_t thread);
```
- thread: 是目标线程的标识符 
- 成功返回 0 失败返回错误码

### 线程属性
- [ ] 待更新

### POSIX信号量
- [ ] 待更新

### 互斥锁
互斥锁(也称互斥量)可以用于保护关键代码段，以确保其独占式的访问，这有点像一个二进制信号量，当进入关键代码段时，我们需要获得互斥锁并将其加锁，这等价于二进制信号量的P操作;当离开关键代码段时，我们需要对互斥锁解锁，以唤醒其他等待该互斥锁的线程，这等价于二进制信号量的V操作。

```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexarr_t* mutexattr);
int pthread_mutex_destroy(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);
```
- mutex: 指向要操作的目标互斥锁
- pthread_mutex_init用于初始化互斥锁 mutexattr参数指定互斥锁的属性 设置为NULL 表示使用默认属性

### 条件变量
如果说互斥锁是用于同步线程对共享数据的访问的话，那么条件变量则是用于在线程之间同步共享数据的值。条件变量提供了一种线程间的通知机制:当某个共享数据达到某个值
的时候，唤醒等待这个共享数据的线程。
```c
#include <pthread.h>
int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* cond_attr);
int pthread_cond_destory(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);
int pthread_cond_signal(pthread_cond_t* cond);
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);
```
- cond: 指向要操作的目标条件变量
- 什么属性啊 什么初始化啊 同互斥锁

### 多线程环境
#### 可重入函数
如果一个函数能被多个线程同时调用且不发生竞态条件，则我们称它是线程安全的(thread safe)，或者说它是可重人函数。Linux库函数只有一小部分是不可重入的

- [ ] 待更新