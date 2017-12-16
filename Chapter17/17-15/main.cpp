#include"../../apue.h"
#include<sys/socket.h>

//为接收/发送描述符定义的控制缓冲区尺寸
#define CONTROLLEN CMSG_LEN(sizeof(int))

//消息控制数据指针
static struct cmsghdr *cmptr=NULL;

int recv_fd(int fd,ssize_t (*usrfunc)(int,const void *,size_t)){
    int newfd,nr,status;
    char *ptr;
    char buf[MAXLINE];
    struct iovec iov[1];
    struct msghdr msg;
    //
    status=-1;
    //
    for(;;){
        //接收数据
        iov[0].iov_base=buf;
        iov[0].iov_len=sizeof(buf);
        //消息接收数据
        msg.msg_iov=iov;
        msg.msg_iovlen=1;
        //消息地址
        msg.msg_name=NULL;
        msg.msg_namelen=0;
        //消息控制数据
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        //接收失败
        if((nr=recvmsg(fd,&msg,0))<0){
            err_sys("recvmsg error");
        }
        //连接断开
        else if(nr==0){
            err_ret("connection closed by server");
            return(-1);
        }
        //遍历整理接收数据
        for(ptr=buf;ptr<&buf[nr];){
            //协议第一位为0
            if(*ptr++==0){
                //协议异常
                if(ptr!=&buf[nr-1]){
                    err_dump("message format error");
                }
                //提取状态位
                status=*ptr&0xFF;
                //如果无错误
                if(status==0){
                    //无文件描述符传递
                    if(msg.msg_controllen!=CONTROLLEN){
                        err_dump("status=0 but no fd");
                    }
                    //存储文件描述符
                    newfd=*(int*)CMSG_DATA(cmptr);
                }
                //存在错误
                else{
                    newfd=-status;
                }
                //清空nr
                nr-=2;
            }
        }
        //如果接收到错误信息，则输出
        if(nr>0&&(*usrfunc)(STDERR_FILENO,buf,nr)!=nr){
            return(-1);
        }
        //如果无误
        if(status>=0){
            return(newfd);
        }
    }
}
