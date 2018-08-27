#include"../17-27/opend.h"
#include<fcntl.h>

void request(char *buf,int nread,int clifd,uid_t uid){
    int newfd;
    // 无结束符
    if(buf[nread-1]!=0){
        // 拼写错误
        snprintf(errmsg,MAXLINE-1,
            "request from uid %d not null terminated: %*.*s\m",
            uid,nread,nread,buf);
        // 发送错误
        send_err(clifd,-1,errmsg);
        // 返回
        return;
    }
    // 输出日志
    log_msg("request: %s,from uid %d",buf,uid);
    // 分析参数
    /* parse the argument,set options */
    if(buf_arg(buf,cli_args)<0){
        // 发送错误
        send_err(clifd,-1,errmsg);
        // 日志
        log_msg(errmsg);
        // 返回
        return;
    }
    // 打开文件
    if((newfd=open(pathname,oflag))<0){
        // 拼写日志
        snprintf(errmsg,MAXLINE-1,"can't open %s: %s\n",
            pathname,strerror(errno));
        // 发送错误
        send_err(clifd,-1,errmsg);
        // 日志
        log_msg(errmsg);
        // 返回
        return;
    }
    // 发送文件描述符
    /* send the descriptor */
    if(send_fd(clifd,newfd)<0){
        log_sys("send_fd error");
    }
    // 日志
    log_msg("sent fd %d over fd %d for %s",newfd,clifd,pathname);
    // 关闭描述符
    close(newfd); /* we're done with descriptor */
}