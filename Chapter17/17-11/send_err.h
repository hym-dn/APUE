#include"../../apue.h"

/**
 * Used when we had planned to send an fd using send_fd().
 * but encountered and error instead. We send the error back
 * using the send_fd()/recv_fd() protocol.
 */
int send_err(int fd,int errcode,const char *msg){
    int n;
    // 错误消息存在
    if((n=strlen(msg))>0){
        // 向管道写错误消息
        if(writen(fd,msg,n)!=n){ /* send the error message */
            return(-1);
        }
    }
    // 设置错误码
    if(errcode>=0){
        errcode=-1; /* must be negative */
    }
    // 发送错误码
    if(send_fd(fd,errcode)<0){
        return(-1);
    }
    // 返回
    return(0);
}