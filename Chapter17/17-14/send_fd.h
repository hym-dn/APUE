#include"../../apue.h"
#include<sys/socket.h>

// 控制消息尺寸
/* size of control buffer to send/recv one file descriptor */
#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr *cmptr=NULL; /* malloc'ed first time */

// 发送文件描述符
/**
 * Pass a file descriptor to another process.
 * If fd<0,then -fd is sent back instead as the error status.
 */
int send_fd(int fd,int fd_to_send){
    // 消息数据
    struct iovec iov[1];
    struct msghdr msg;
    char buf[2]; /* send_fd()/recv_fd() 2-byte protocol */
    // 控制协议
    iov[0].iov_base=buf;
    iov[0].iov_len=2;
    // 消息数据
    msg.msg_iov=iov;
    msg.msg_iovlen=1;
    // 消息目标地址
    msg.msg_name=NULL;
    msg.msg_namelen=0;
    // 文件描述符非法
    if(fd_to_send<0){
        // 消息控制数据
        msg.msg_control=NULL;
        msg.msg_controllen=0;
        // 状态位
        buf[1]=-fd_to_send; /* nonzero status means error */
        // 状态位溢出
        if(buf[1]==0){
            buf[1]=1; /* -256,etc,would screw up protocol */
        }
    }
    // 文件描述符合法
    else{
        // 分配控制数据
        if((cmptr==NULL)&&((cmptr=(struct 
            cmsghdr *)malloc(CONTROLLEN))==NULL)){
            return(-1);
        }
        // 配置控制数据
        cmptr->cmsg_level=SOL_SOCKET;
        cmptr->cmsg_type=SCM_RIGHTS;
        cmptr->cmsg_len=CONTROLLEN;
        // 绑定控制数据
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        // 设置传递文件描述符
        *(int*)CMSG_DATA(cmptr)=fd_to_send; /* the fd to pass */
        // 清除错误状态
        buf[1]=0; /* zero status means OK */
    }
    // 协议位
    buf[0]=0; /* null byte flag to recv_fd() */
    // 发送描述符
    if(sendmsg(fd,&msg,0)!=2){
        return(-1);
    }
    // 返回
    return(0);
}
