#include"../17-18/open.h"
#include<sys/uio.h> /* struct iovec */

// 发送“名字”以及“标志”到服务器，并且读取服务器返回的文件描述符
/**
 * Open the file by sending the "name" and "oflag" to the 
 * connection server and reading a file descriptor back.
 */
int csopen(char *name,int oflag){
    int len;
    char buf[10];
    struct iovec iov[3];
    static int csfd=-1;
    // 如果尚未连接到服务器
    if(csfd<0){ /* open connection to conn server */
        // 连接服务器
        if((csfd=cli_conn(CS_OPEN))<0){
            err_sys("cli_conn error");
        }
    }
    // 形成标志
    sprintf(buf," %d",oflag); /* oflag to ascii */
    // 形成数据
    iov[0].iov_base=(void*)(CL_OPEN " "); /* string concatenation */
    iov[0].iov_len=strlen(CL_OPEN)+1;
    iov[1].iov_base=name;
    iov[1].iov_len=strlen(name);
    iov[2].iov_base=buf;
    iov[2].iov_len=strlen(buf)+1; /* null always sent */
    // 数据长度
    len=iov[0].iov_len+iov[1].iov_len+iov[2].iov_len;
    // 向服务器发送数据
    if(writev(csfd,&iov[0],3)!=len){
        err_sys("writev error");
    }
    // 返回文件描述符
    return(recv_fd(csfd,write));
}