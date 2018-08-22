/* 为UNIX域套接字绑定地址 */

#include"../../apue.h"
#include<sys/socket.h>
#include<sys/un.h>

int main(void){
    // 套接字、尺寸
    int fd,size;
    // 地址
    struct sockaddr_un un;
    // 设置协议族
    un.sun_family=AF_UNIX;
    // 拷贝文件路径
    strcpy(un.sun_path,"foo.socket");
    // 创建套接字
    if((fd=socket(AF_UNIX,SOCK_STREAM,0))<0){
        err_sys("socket failed");
    }
    // 计算un中有效数据的实际尺寸
    // offsetof计算结构体中指定成员的偏移量
    size=offsetof(struct sockaddr_un,sun_path)+
        strlen(un.sun_path);
    // 绑定套接字
    if(bind(fd,(struct sockaddr *)&un,size)<0){
        err_sys("bind failed");
    }
    // 打印绑定信息
    printf("UNIX domain socket bound\n");
    // 结束进程
    exit(0);
}
