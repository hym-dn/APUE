#include"../17-18/open.h"
#include<sys/uio.h>

int csopen(char *name,int oflag){
    int len;
    char buf[10];
    struct iovec iov[3];
    static int csfd=-1;
    //如果尚未与服务器建立连接
    if(csfd<0){
        //连接失败
        if((csfd=cli_conn(CS_OPEN))<0){
            err_sys("cli_conn error");
        }
    }
    //形成标志
    sprintf(buf," %d",oflag);
    //形成数据
    iov[0].iov_base=CL_OPEN "";
    iov[0].iov_len=strlen(CL_OPEN)+1;
    iov[1].iov_base=name;
    iov[1].iov_len=strlen(name);
    iov[2].iov_base=buf;
    iov[2].iov_len=strlen(buf)+1;
    //计算数据长度
    len=iov[0].iov_len+iov[1].iov_len+iov[2].iov_len;
    //向服务器发送数据
    if(writev(csfd,&iov[0],3)!=len){
        err_sys("writev error");
    }
    //接收并返回文件描述符
    return(recv_fd(csfd,write));
}

