#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>


/*
*@  socket 创建socket
*@  int socket(int domain, int type, int protocol)
*@  domain      使用那个底层协议
*@  type        指定服务类型 TCP流服务(SOCK_STREAM)  UDP数据报(SOCK_UGRAM)
*@  protocol    基本为 0
*@  成功返回一个socket文件描述符 失败返回 -1 并设置errno
*/

/*
*@  bind 命名socket
*@  int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen)
*@  bind 将 my_addr 所指的socket地址分配给未命名的 sockfd 文件描述符 addrlen 参数为 socket 的地址长度
*@  成功返回 0 失败返回 -1 并设置 errno 常见errno有 EACCES EADDEINUSE
*@  EACCES: 被绑定的地址是受保护的地址 仅超级用户能访问
*@  EADDRINUSE: 被绑定的地址正在使用
*/

/*
*@  listen 监听socket
*@  int listen(int sockfd, int backlog)
*@  sockfd参数指定被监听的socket 
*@  backlog参数提示内核监听队列的最大长度 如果超过则不受理新的客户连接 
*/

static bool stop = false;

/* SIGTERM 信号的处理函数 触发时结束主程序中的循环 */
static void handle_term(int sig){
    stop = true;
}

/* 发现 能够建立连接的数是 backlog + 1 其他连接都处于 SYN_RCVD状态 */
int main(int argc, char* argv[]){
    signal(SIGTERM, handle_term);

    if(argc < 3){
        printf("usage: %s ip_address port_number baclog\n", basename( argv[0] ));
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    /* 创建一个IPv4 socket地址 */
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);
    // address.sin_addr.s_addr = inet_addr(ip); 我通常使用的操作

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, backlog);
    assert(ret != -1);

    /* 循环等待连接 知道有 SIGTERM 信号将它中断 */
    while(!stop){
        sleep(1);
    }

    /* 关闭sock */
    close(sock);
    return 0;
}