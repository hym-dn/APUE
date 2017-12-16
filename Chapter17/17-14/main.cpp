#include"../../apue.h"
#include<sys/socket.h>

//发送文件描述符，所需的控制缓冲区长度
#define CONTROLLEN CMSG_LEN(sizeof(int))

//
static struct cmsghdr *cmptr=NULL;

//向另一个进程发送文件描述符
int send_fd(int fd,int fd_to_send){
    struct iovec iov[1];
    struct msghdr msg;
    char buf[2];
    //发送数据
    iov[0].iov_base=buf;
    iov[0].iov_len=2;
    //消息数据
    //数据
    msg.msg_iov=iov;
    msg.msg_iovlen=1;
    //地址
    msg.msg_name=NULL;
    msg.msg_namelen=0;
    //描述符非法
    if(fd_to_send<0){
        //消息控制数据
        msg.msg_control=NULL;
        msg.msg_controllen=0;
        //状态位
        buf[1]=-fd_to_send;
        //如果fd_to_send溢出
        if(buf[1]==0){
            buf[1]=1;
        }
    }
    //描述符合法
    else{
        //为控制数据分配内存
        if((cmptr==NULL)&&((cmptr=(struct 
            cmsghdr *)malloc(CONTROLLEN))==NULL)){
            return(-1);
        }
        //控制数据
        cmptr->cmsg_level=SOL_SOCKET;
        cmptr->cmsg_type=SCM_RIGHTS;
        cmptr->cmsg_len=CONTROLLEN;
        //消息控制数据
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        *(int*)CMSG_DATA(cmptr)=fd_to_send;
        //清除错误状态
        buf[1]=0;
    }
    //协议位
    buf[0]=0;
    //发送描述符
    if(sendmsg(fd,&msg,0)!=2){
        return(-1);
    }
    //返回
    return(0);
}
