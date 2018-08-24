#include"../../apue.h"
#include<sys/socket.h> /* struct msghdr */
#include<sys/un.h>

// 定义凭证
#if defined(SCM_CREDS) /* BSD interface */
#define CREDSTRUCT cmsgcred
#define CR_UID cmcred_uid
#define CREDOPT LOCAL_PREECRED
#define SCM_CREDTYPE SCM_CREDS
#elif defined(SCM_CREDENTIALS) /* Linux interface */
#define CREDSTRUCT ucred
#define CR_UID uid
#define CREDOPT SO_PASSCRED
#define SCM_CREDTYPE SCM_CREDENTIALS
#else
#error passing credentials is unsupported!
#endif

// 文件描述符缓冲区尺寸
// size of control buffer to send/recv one file descriptor */
#define RIGHTSLEN CMSG_LEN(sizeof(int))
// 凭证缓冲区尺寸
#define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
// 控制消息尺寸
#define CONTROLLEN (RIGHTSLEN+CREDSLEN)

// 消息结构
static struct cmsghdr *cmptr=NULL; /* malloc'ed first time */

// 接收文件描述符
/**
 * Receive a file descriptor from a server process. Also, any data
 * received is passed to (*userfunc)(STDERR_FILENO,buf,nbytes).
 * We have a 2-byte protocol for receiving the fd from send_fd().
 */
int recv_ufd(int fd,uid_t *uidptr,ssize_t(*userfunc)(int,const void *,size_t)){
    // 控制消息
    struct cmsghdr *cmp;
    // 凭证
    struct CREDSTRUCT *credp;
    char *ptr;
    // 控制协议
    char buf[MAXLINE];
    struct iovec iov[1];
    // 消息
    struct msghdr msg;
    int nr;
    // 文件描述符
    int newfd=-1;
    // 状态
    int status=-1;
    // 启动接收凭证
#if defined(CREDOPT)
    const int on=1;
    if(setsockopt(fd,SOL_SOCKET,CREDOPT,&on,sizeof(int))<0){
        err_ret("setsockopt error");
        return(-1);
    }
#endif
    // 循环接收消息
    for(;;){
        // 设置消息中的数据消息
        iov[0].iov_base=buf;
        iov[0].iov_len=sizeof(buf);
        msg.msg_iov=iov;
        msg.msg_iovlen=1;
        // 设置消息地址
        msg.msg_name=NULL;
        msg.msg_namelen=0;
        // 分配控制数据
        if(cmptr==NULL&&(cmptr=(struct cmsghdr *)malloc(CONTROLLEN))==NULL){
            return(-1);
        }
        // 设置消息控制数据
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        // 接收消息
        if((nr=recvmsg(fd,&msg,0))<0){
            // 出错
            err_ret("recvmsg error");
            // 返回
            return(-1);
        }
        // 读取为空（连接断开）
        else if(nr==0){
            // 出错
            err_ret("connection closed by server");
            // 返回
            return(-1);
        }
        // 分析接收数据
        for(ptr=buf;ptr<&buf[nr];){
            // 第一个字节为0
            if(*ptr++==0){
                // 协议格式错误
                if(ptr!=&buf[nr-1]){
                    err_dump("message format error");
                }
                // 计算状态
                status=*ptr&0xFF;
                // 如果无错误状态
                if(0==status){
                    // 如果传递文件描述符尺寸非法
                    if(msg.msg_controllen!=CONTROLLEN){
                        err_dump("status=0 but no fd");
                    }
                    // 遍历消息中控制数据
                    for(cmp=CMSG_FIRSTHDR(&msg);cmp!=NULL;cmp=CMSG_NXTHDR(&msg,cmp)){
                        // 消息协议非法
                        if(cmp->cmsg_level!=SOL_SOCKET){
                            continue;
                        }
                        // 消息分类
                        switch(cmp->cmsg_type){
                        // 描述符
                        case SCM_RIGHTS:
                            // 保存描述符
                            newfd=*(int*)CMSG_DATA(cmp);
                            // 跳出
                            break;
                        // 凭证
                        case SCM_CREDTYPE:
                            // 保存凭证
                            credp=(struct CREDSTRUCT *)CMSG_DATA(cmp);
                            // 保存用户ID
                            *uidptr=credp->CR_UID;
                        }
                    }
                }
                // 状态错误
                else{
                    newfd=-status;
                }
                // 设置消息数量
                nr-=2;
            }
        }
        // 如果存在错误消息，则输出
        if(nr>0&&(*userfunc)(STDERR_FILENO,buf,nr)!=nr){
            return(-1);
        }
        // 返回
        if(status>=0){
            return(newfd);
        }
    }
}
