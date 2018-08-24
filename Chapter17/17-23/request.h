#include"../17-21/opend.h"
#include<fcntl.h>

void request(char *buf,int nread,int fd){
    int newfd;
    // 如果输入指令无终止符
    if(buf[nread-1]!=0){
        // 生成错误信息
        snprintf(errmsg,MAXLINE-1,"request not "
            "null terminated: %*.*s\n",nread,
            nread,buf);
        // 发送错误
        send_err(fd,-1,errmsg);
        // 返回
        return;
    }
    // 分解参数
    if(buf_args(buf,cli_args)<0){ /* parse args & set options */
        // 发送错误
        send_err(fd,-1,errmsg);
        // 返回
        return;
    }
    // 打开文件描述符
    if((newfd=open(pathname,oflag))<0){
        // 生成错误
        snprintf(errmsg,MAXLINE-1,"can't open %s: %s\n",
            pathname,strerror(errno));
        // 发送错误
        send_err(fd,-1,errmsg);
        // 返回
        return;
    }
    // 发送描述符
    if(send_fd(fd,newfd)<0){ /* send the descriptor */
        err_sys("send_fd error");
    }
    // 关闭描述符
    close(newfd); /* we're done with descriptor */
}