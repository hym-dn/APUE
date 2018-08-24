#include"../../apue.h"
#include<sys/socket.h>

// 定义凭证结构
#if defined(SCM_CREDS) /* BSD interface */
#define CREDSTRUCT cmsgcred
#define SCM_CREDTYPE SCM_CREDS
#elif defined(SCM_CREDENTIALS) /* Linux interface*/
#define CREDSTRUCT ucred
#define SCM_CREDTYPE SCM_CREDENTIALS
#else
#error passing credentials is unsupported!
#endif

// 定义发送文件描述符控制消息长度
/* size of control buffer to send/recv one file descriptor */
#define RIGHTSLEN CMSG_LEN(sizeof(int))
// 定义发送凭证控制消息长度
#define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
// 定义控制消息整体长度
#define CONTROLLEN (RIGHTSLEN+CREDSLEN)

// 控制消息
static struct cmsghdr *cmptr=NULL; /* malloc'ed first time */

//发送一个文件描述符到另一个进程
/**
 * Pass a file descriptor to another process.
 * If fd<0,then -fd is sent back instead as the error status.
 */
int send_fd(int fd,int fd_to_send){
    // 凭证
    struct CREDSTRUCT *credp;
    // 控制消息
    struct cmsghdr *cmp;
    // 数据缓冲区
    struct iovec iov[1];
    // 消息
    struct msghdr msg;
    // 控制协议
    char buf[2];
    // 设置控制协议
    iov[0].iov_base=buf;
    iov[0].iov_len=2;
    // 设置消息数据
    msg.msg_iov=iov;
    msg.msg_iovlen=1;
    // 设置消息地址
    msg.msg_name=NULL;
    msg.msg_namelen=0;
    // 设置消息标志
    msg.msg_flags=0;
    // 文件描述符非法
    if(fd_to_send<0){
        // 清除控制数据
        msg.msg_control=NULL;
        msg.msg_controllen=0;
        // 设置错误状态
        buf[1]=-fd_to_send; /* nonzero status means error */
        // 溢出处理
        if(buf[1]==0){
            buf[1]=1; /* -256,etc.would screw up protocol */
        }
    }
    // 文件描述符合法
    else{
        // 如果控制消息尚未创建，则创建之
        if(cmptr==NULL&&(cmptr=(struct cmsghdr *)malloc(CONTROLLEN))==NULL){
            return(-1);
        }
        // 设置控制消息
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        // 设置发送的文件描述符
        cmp=cmptr;
        cmp->cmsg_level=SOL_SOCKET;
        cmp->cmsg_type=SCM_RIGHTS;
        cmp->cmsg_len=RIGHTSLEN;
        *(int*)CMSG_DATA(cmp)=fd_to_send; /* the fd to pass */
        // 设置凭证
        cmp=CMSG_NXTHDR(&msg,cmp);
        cmp->cmsg_level=SOL_SOCKET;
        cmp->cmsg_type=SCM_CREDTYPE;
        cmp->cmsg_len=CREDSLEN;
        credp=(struct CREDSTRUCT *)CMSG_DATA(cmp);
#if defined(SCM_CREDENTIALS)
        credp->uid=geteuid();
        credp->gid=getgid();
        credp->pid=getpid();
#endif
        // 清除错误
        buf[1]=0; /* zero status means OK */
    }
    // 设置协议第一位
    buf[0]=0; /* null byte flag to recv_ufd() */
    // 发送消息
    if(sendmsg(fd,&msg,0)!=2){
        return(-1);
    }
    // 返回
    return(0);
}