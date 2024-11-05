#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <cerrno>

/*
*@  accept 接受连接
*@  int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*@  sockfd参数是执行过listen系统调用的监听socket 
*@  addr参数是用来获取被接受连接的远端socket地址 该地址的长度由addrlen参数指出
*@  accept成功时返回一个新的连接socket 
*@  该连接唯一地标识了被接受的这个连接 服务器可以通过读写该socket 来与被连接的客户端通信
*@  成功返回一个新的socket 失败返回 -1并errno
*/

/*
*@  connect 发起连接
*@  int connect(int sockfd, const struct sockaddr *serve_addr, socklen_t addrlen)
*@  sockfd参数由socket系统调用返回一个socket
*@  serv_addr参数是服务器监听的socket地址
*@  addrlen参数则是指定这个地址的长度
*@  成功返回 0 失败返回 -1并设置errno 其中两种常见的errno ECONNREFUSED 和 ETIMEDOUT
*@  ECONNREFUSED: 目标端口不存在 连接被拒绝
*@  ETIMEDOUT: 连接超时
*/

/*
*@  close 关闭连接
*@  int close(int fd);
*@  fd参数是待关闭的socket 不过 close系统调用并非总是立即关闭一个连接 而是将fd的引用次数减1
*@  只有当fd的引用次数为0时 才是真正关闭连接
*/

/*
*@  shutdown
*@  int shutdown(int sockfd, int howto)
*@  sockfd参数是待关闭的socket howto参数决定了shutdowm的行为
*@  SHUT_RD: 关闭sockfd上读的这一半 socket不能进行读操作 并且接受缓冲区中的数据都被丢弃
*@  SHUT_WR: 关闭sockfd上写的这一半 发送缓冲区中的数据在真正关闭前全部发送出去 不能再进行写操作 处于半关闭
*@  SHUT_RDWR: 同时关闭sockfd上的读写
*@  成功时返回0 失败则返回 -1并设置errno
*/

int main(int argc, char* argv[]){
    if(argc < 2){   // basename() 提取文件名
        printf("usage: %s ip_address port_number baclog\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);    
    /* 暂停20 秒以等待客户端连接和相关操作(掉线或者退出)完成 */
    sleep(20);
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    if(connfd < 0){
        printf("error is %d\n", errno);
    }else{
        /* 接受连接成功则打印客户端的IP地址和端口号 */
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip: %s and port: %d\n",
                inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN),
                ntohs(client.sin_port));
        close(connfd);
    }
    close(sock);
    return 0;
}