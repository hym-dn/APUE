#include"../../apue.h"
#include<errno.h>
#include<sys/socket.h>

/**
 * type - 套接子类型。
 * addr - 地址信息。
 * alen - 地址信息尺寸。
 * qlen - 允许的连接数。
 */
int initserver(int type,const struct sockaddr *addr,socklen_t alen,int qlen){
    // 记录套接字
    int fd;
    // 记录错误
    int err=0;
    // 创建一个套接子
    if((fd=socket(addr->sa_family,type,0))<0){
        return(-1);
    }
    // 绑定地址
    if(bind(fd,addr,alen)<0){
        err=errno;
        goto errout;
    }
    // 如果套接字面向连接
    if(SOCK_STREAM==type||SOCK_SEQPACKET==type){
        // 监听套接字
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