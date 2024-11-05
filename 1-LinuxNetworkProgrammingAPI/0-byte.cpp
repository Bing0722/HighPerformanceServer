#include <netinet/in.h>
#include <stdio.h>

// 小端存储又称主机字节序
void byteOrder() {
    union {
        short value;
        char union_bytes[sizeof(short)];
    } test;
    test.value = 0x0102; // 设置一个十六进制值
    if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2)) {
        printf("big endian\n");
    } else if ((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1)) {
        printf("little endian\n");
    } else {
        printf("unknow...\n");
    }
    
}

/*
*@  字节转换
*@  htonl: unsigned long int    host to network long
*@  htons: unsigned short int   host to network short
*@  ntohl: unsigned long int    network to host long
*@  ntohs: unsigned short int   network to host short
*/

void changeBytes() {
    short value = 8888;
    printf("htons: %d\n", htons(value));
    printf("ntohs: %d\n", ntohs(value));
}

int main() {
    // byteOrder();
    changeBytes();
    return 0;
}
