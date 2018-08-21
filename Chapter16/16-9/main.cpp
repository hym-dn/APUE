#include"../../apue.h"
#include<errno.h>
#include<sys/socket.h>

int initserver(int type,const struct sockaddr *addr,
    socklen_t alen,int qlen){
    int fd,err;
    // 地址复用开关
    int reuse=1;
    // 创建套接字
    if((fd=socket(addr->sa_family,type,0))<0){
        return(-1);
    }
    // 设置套接字地址可以复用
    if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,
        &reuse,sizeof(int))<0){
        err=errno;
        goto errout;
    }
    // 绑定套接字
    if(bind(fd,addr,alen)<0){
        err=errno;
        goto errout;
    }
    // 监听套接字
    if(type==SOCK_STREAM||type==SOCK_SEQPACKET){
        if(listen(fd,qlen)<0){
            err=errno;
            goto errout;
        }
    }
    // 返回套接字
    return(fd);
// 错误处理
errout:
    close(fd);
    errno=err;
    return(-1);
}