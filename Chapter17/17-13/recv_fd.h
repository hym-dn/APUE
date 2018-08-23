#include"../../apue.h"
#include<stropts.h>

/**
 * Receive a file descriptor from another process (a server).
 * In addition,any data received from the server is passed to
 * (*userfunc)(STDERR_FILENO,buf,nbytes). We have a 2-byte protocol
 * for receiving the fd from send_fd().
 */
int recv_fd(int fd,ssize_t(*userfunc)(int,const void *,size_t)){
    int newfd,nread,flag,status;
    char *ptr;
    char buf[MAXLINE];
    struct strbuf dat;
    struct strrecvfd recvfd;
    status=-1;
    for(;;){
        dat.buf=buf;
        dat.maxlen=MAXLINE;
        flag=0;
        // 从流首读STREAMS消息
        if(getmsg(fd,NULL,&dat,&flag)<0){
            err_sys("getmsg error");
        }
        // 记录读取长度
        nread=dat.len;
        // 读到文件尾流断开
        if(nread==0){
            err_ret("connection closed by server");
            return(-1);
        }
        // 遍历读取缓冲区
        /**
         * See if this is the final data with null & status.
         * Null must be next to last byte of buffer,status
         * byte is last byte. Zero status means there must
         * be a file descriptor to receive.
         */
        for(ptr=buf;ptr<&buf[nread];){
            // 当前字符为0
            if(*ptr++==0){
                // 协议格式错误
                if(ptr!=&buf[nread-1]){
                    err_dump("message format error");
                }
                // 读取错误状态
                status=*ptr&0xFF;
                // 无错误状态
                if(0==status){
                    // 读取文件描述符
                    if(ioctl(fd,I_RECVFD,&recvfd)<0){
                        return(-1);
                    }
                    // 记录新的文件描述符
                    newfd=recvfd.fd;
                }
                // 错误状态
                else{
                    newfd=-status;
                }
                // 避免下次循环
                nread-=2;
            }
        }
        // 如果当前读取的是错误消息
        if(nread>0){
            if((*userfunc)(STDERR_FILENO,buf,nread)!=nread){ // 调用显示错误消息
                return(-1);
            }
        }
        // 返回文件描述符
        if(status>=0){ /* final data has arrived */
            return(newfd); /* descriptor,or -status */
        }
    }
}
