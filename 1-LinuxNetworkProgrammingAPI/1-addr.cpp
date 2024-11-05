#include <arpa/inet.h>
#include <stdio.h>

/*
*@ ip地址转换 ipv4
*@ in_addr_t inet_addr(const char* strptr)  将点分十进制转换成网络字节序
*@ int       inet_aton(const char* cp, struct ip_addr* inp) 功能同 inet_addr
*@ char*     inet_ntoa(struct in_addr in)   将网络字节序转换成点分十进制
*/

/*
*@  
*@  int inet_pton(int af, const char* src, void* dst)
*@  af指定地址族 inet_pton 将字符串表示的IP地址src(点分十进制)转换成网络字节序IP地址存储于dst
*@  const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt)
*@  与上面相反 cnt指定目标存储单元的大小 一般是下面两个宏
*@  #define INET_ADDRESTRLEN 16
*@  #define INET6_ADDRESTRLEN 46
*/

int main(){

    return 0;
}