#include"../../apue.h"
#include<sys/socket.h> /* struct msghdr */

// 控制缓冲区尺寸
/* sizeof control buffer to send/recv on file descriptor */
#define CONTROLLEN CMSG_LEN(sizeof(int))

// 控制消息
static struct cmsghdr *cmptr=NULL; /* malloc'ed first time */

/**
 * Receive a file descriptor from a server process. Also, any data
 * received is passed to (*userfunc)(STDERR_FILENO,buf,nbytes).We 
 * have a 2-byte protocol for receiving the fd from send_fd().
 */
int recv_fd(int fd,ssize_t (*usrfunc)(int,const void *,size_t)){
    int newfd,nr,status;
    char *ptr;
    // 接收数据缓冲区
    char buf[MAXLINE];
    struct iovec iov[1];
    struct msghdr msg;
    // 错误状态
    status=-1;
    // 循环接收数据
    for(;;){
        // 接收数据缓冲区
        iov[0].iov_base=buf;
        iov[0].iov_len=sizeof(buf);
        // 设置消息数据
        msg.msg_iov=iov;
        msg.msg_iovlen=1;
        // 设置消息地址
        msg.msg_name=NULL;
        msg.msg_namelen=0;
        // 设置消息控制数据
        if(cmptr==NULL&&(cmptr=(cmsghdr*)malloc(CONTROLLEN))==NULL){
            return(-1);
        }
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;       
        // 接收消息失败
        if((nr=recvmsg(fd,&msg,0))<0){
            err_sys("recvmsg error");
        }
        // 连接断开
        else if(nr==0){
            err_ret("connection closed by server");
            return(-1);
        }
        //遍历整理接收数据
        /**
         * See if this is the final data with null & status. Null
         * is next to last byte of buffer; status byte is last byte.
         * Zero status means there is file descriptor to receive.
         */
        for(ptr=buf;ptr<&buf[nr];){
            // 第一位为0，描述符传递协议
            if(*ptr++==0){
                // 协议异常
                if(ptr!=&buf[nr-1]){
                    err_dump("message format error");
                }
                // 提取状态位
                status=*ptr&0xFF;
                // 如果无错误
                if(status==0){
                    // 收到描述符消息异常
                    if(msg.msg_controllen!=CONTROLLEN){
                        err_dump("status=0 but no fd");
                    }
                    // 存储文件描述符
                    newfd=*(int*)CMSG_DATA(cmptr);
                }
                // 存在错误
                else{
                    newfd=-status;
                }
                // 清空nr
                nr-=2;
            }
        }
        // 如果收到的是错误消息，则输出
        if(nr>0&&(*usrfunc)(STDERR_FILENO,buf,nr)!=nr){
            return(-1);
        }
        // 如果无误
        if(status>=0){
            return(newfd);
        }
    }
}