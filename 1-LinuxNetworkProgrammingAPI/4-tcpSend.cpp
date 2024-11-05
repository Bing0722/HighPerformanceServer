#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
*@ TCP数据读写
*@  ssize_t recv(int sockfd, void *buf, size_t len, int flags)
*@  ssize_t send(int sockfd, const void* buf, size_t len, int flags)
*@  recv读取sockfd上的数据 buf和len参数分别指定读缓冲区的位置和大小 flags 可选值 一般为0
*@  recv成功时返回实际读取到的数据的长度 它可能小于len 
*@  如果recv返回0 表示对方关闭了连接 如果出错返回 -1并设置errno
*@  send和recv相同
*/

/*
*@  flags参数
*@      选项名                                  含义                                                     send  recv 
*@  MSG_CONFIRM:    指示数据链路层协议持续监听对方的回应 知道得到答复 仅适用于SOCK_DGRAM和 SOCK_RAW 类型的socket    Y     N
*@  MSG_DONTROUTE:  不查看路由表 直接将数据发送给本地局域网内的主机 表示发送者确切知道目标主机就在本地网络上            Y     N
*@  MSG_DONTWAIT:   对socket的此次操作将是非阻塞的                                                           Y      Y
*@  MSG_MORE:       告诉内核还有更多数据要发送 内核将超时等待新数据写入tcp发送缓冲区后一并发送 防止发送过小 提高速率    Y      N
*@  MSG_WAITALL:    读操作仅在读取到指定数量的字节后才返回                                                      N     Y
*@  MSG_PEEK:       窥探读缓存中的数据 此处读操作不会导致这些数据被清除                                           Y     Y
*@  MSG_OOB:        发送或接受紧急数据                                                                      Y      Y
*@  MSG_NOSIGNAL:   往读端关闭的管道或者socket连接中写数据时不引发SIGPIPE信号                                    Y      N
*/

int main(int argc, char* argv[]){
    if(argc < 2){   // basename() 提取文件名
        printf("usage: %s ip_address port_number baclog\n", basename(argv[0]));
        return 1;
    }    
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_address.sin_addr);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if(connect(sockfd,(struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        printf("connect failed\n");
    }else{
        const char* oob_data = "abc";
        const char* normal_data = "123";
        send(sockfd, normal_data, strlen(normal_data), 0);
        send(sockfd, oob_data, strlen(oob_data), MSG_OOB);
        send(sockfd, normal_data, strlen(normal_data), 0);
    }
    close(sockfd);
    return 0;
}
