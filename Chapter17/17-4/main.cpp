#include"../../apue.h"
#include<stropts.h>

/**
 * 等待客户端连接，返回系统自动创建的管道文件描述符号
 * 以及客户端有效用户ID。
 * listenfd - 监听管道文件描述符。
 * uidptr -客户端有效用户ID。
 * 如果函数执行成功返回自动创建的管道文件描述符，否则
 * 返回值小于0。
 */
int serv_accept(int listenfd,uid_t *uidptr){
    //客户端连接信息结构
    struct strrecvfd recvfd;
    //等待客户端连接抵达
    if(ioctl(listenfd,I_RECVFD,&recvfd)<0){
        return(-1);
    }
    //存储管道文件描述符
    if(uidptr!=NULL){
        *uidptr=recvfd.uid;
    }
    //返回管道文件描述符
    return(recvfd.fd);
}
