#include"../../apue.h"
#include<sys/socket.h>

//定义发送凭证结构
#if defined(SCM_CREDS)
#define CREDSTRUCT cmsgcred
#define SCM_CREDTYPE SCM_CREDS
#elif defined(SCM_CREDENTIALS)
#define CREDSTRUCT ucred
#define SCM_CREDTYPE SCM_CREDENTIALS
#else
#error passing credentials is unsupported!
#endif

//定义发送描述符长度
#define RIGHTSLEN CMSG_LEN(sizeof(int))
//定义发送凭证长度
#define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
//定义控制长度
#define CONTROLLEN (RIGHTSLEN+CREDSLEN)

//消息指针
static struct cmsghdr *cmptr=NULL;

//发送一个文件描述符到另一个进程
int send_fd(int fd,int fd_to_send){
    struct CREDSTRUCT *credp;
    struct cmsghdr *cmp;
    struct iovec iov[1];
    struct msghdr msg;
    char buf[2];
    //设置消息发送的数据
    iov[0].iov_base=buf;
    iov[0].iov_len=2;
    msg.msg_iov=iov;
    msg.msg_iovlen=1;
    //设置消息地址
    msg.msg_name=NULL;
    msg.msg_namelen=0;
    //设置消息标志
    msg.msg_flags=0;
    //如果文件描述符非法
    if(fd_to_send<0){
        //清除控制数据
        msg.msg_control=NULL;
        msg.msg_controllen=0;
        //设置错误状态
        buf[1]=-fd_to_send;
        //溢出处理
        if(buf[1]==0){
            buf[1]=1;
        }
    }
    //如果文件描述符合法
    else{
        //如果消息尚未创建，则创建之
        if(cmptr==NULL&&(cmptr=(struct cmsghdr *)malloc(CONTROLLEN))==NULL){
            return(-1);
        }
        //设置消息控制位
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        //设置发送的文件描述符
        cmp=cmptr;
        cmp->cmsg_level=SOL_SOCKET;
        cmp->cmsg_type=SCM_RIGHTS;
        cmp->cmsg_len=RIGHTSLEN;
        *(int *)CMSG_DATA(cmp)=fd_to_send;
        //设置凭证
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
        //清除错误状态
        buf[1]=0;
    }
    //设置空字节
    buf[0]=0;
    //发送消息
    if(sendmsg(fd,&msg,0)!=2){
        return(-1);
    }
    //返回
    return(0);
}
