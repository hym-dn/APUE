#include"../../apue.h"

int send_err(int fd,int errcode,const char *msg){
    int n;
    //存在错误消息
    if((n=strlen(msg))>0){
        //写错误消息
        if(writen(fd,msg,n)!=n){
            return(-1);
        }
    }
    //
    if(errcode>=0){
        errcode=-1;
    }
    //
    if(send_fd(fd,errcode)<0){
        return(-1);
    }
    //返回
    return(0);
}
