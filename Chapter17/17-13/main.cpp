#include"../../apue.h"
#include<stropts.h>

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
        //从流首读STREAMS消息
        if(getmsg(fd,NULL,&dat,&flag)<0){
            err_sys("getmsg error");
        }
        //记录读取长度
        nread=dat.len;
        //读取长度为0
        if(nread==0){
            err_ret("connection closed by server");
            return(-1);
        }
        //
        for(ptr=buf;ptr<&buf[nread];){
            //当前字符为0
            if(*ptr++==0){
                //格式错误
                if(ptr!=&buf[nread-1]){
                    err_dump("message format error");
                }
                //读取状态
                status=*ptr&0xFF;
                //如果无误
                if(0==status){
                    //接受新的文件描述符
                    if(ioctl(fd,I_RECVFD,&recvfd)<0){
                        return(-1);
                    }
                    //记录新的文件描述符
                    newfd=recvfd.fd;
                }
                //如果有误
                else{
                    newfd=-status;
                }
                nread-=2;
            }
        }
        //输出读取内容
        if(nread>0){
            if((*userfunc)(STDERR_FILENO,buf,nread)!=nread){
                return(-1);
            }
        }
        //返回文件描述符
        if(status>=0){
            return(newfd);
        }

    }
}
