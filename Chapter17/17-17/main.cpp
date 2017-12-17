#include"../../apue.h"
#include<sys/socket.h>
#include<sys/un.h>

//定义凭证
#if defined(SCM_CREDS)
#define CREDSTRUCT cmsgcred
#define CR_UID cmcred_uid
#define SCM_CREDTYPE SCM_CREDS
#elif defined(SCM_CREDENTIALS)
#define CREDSTRUCT ucred
#define CR_UID uid
#define CREDOPT SO_PASSCRED
#define SCM_CREDTYPE SCM_CREDENTIALS
#else
#error passing credentials is unsupported!
#endif

//描述符消息长度
#define RIGHTSLEN CMSG_LEN(sizeof(int))
//凭证消息长度
#define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
//控制消息总长度
#define CONTROLLEN (RIGHTSLEN+CREDSLEN)

static struct cmsghdr *cmptr=NULL;

//接收文件描述符
int recv_ufd(int fd,uid_t *uidptr,ssize_t(*userfunc)(int,const void *,size_t)){
    struct cmsghdr *cmp;
    struct CREDSTRUCT *credp;
    char *ptr;
    char buf[MAXLINE];
    struct iovec iov[1];
    struct msghdr msg;
    int nr;
    int newfd=-1;
    int status=-1;
#if defined(CREDOPT)
    const int on=1;
    if(setsockopt(fd,SOL_SOCKET,CREDOPT,&on,sizeof(int))<0){
        err_ret("setsockopt error");
        return(-1);
    }
#endif
    for(;;){
        //设置消息数据
        iov[0].iov_base=buf;
        iov[0].iov_len=sizeof(buf);
        msg.msg_iov=iov;
        msg.msg_iovlen=1;
        //设置消息地址
        msg.msg_name=NULL;
        msg.msg_namelen=0;
        //分配控制数据
        if(cmptr==NULL&&(cmptr=(struct cmsghdr *)malloc(CONTROLLEN))==NULL){
            return(-1);
        }
        //设置控制数据
        msg.msg_control=cmptr;
        msg.msg_controllen=CONTROLLEN;
        //接收消息
        if((nr=recvmsg(fd,&msg,0))<0){
            //出错
            err_ret("recvmsg error");
            //返回
            return(-1);
        }
        //读取为空
        else if(nr==0){
            //出错
            err_ret("connection closed by server");
            //返回
            return(-1);
        }
    }/*for(;;)*/
    //分析接收数据
    for(ptr=buf;ptr<&buf[nr];){
        //第一个字节为空
        if(*ptr++==0){
            //协议格式错误
            if(ptr!=&buf[nr-1]){
                err_dump("message format error");
            }
            //取状态
            status=*ptr&0xFF;
            //如果状态无误
            if(0==status){
                //如果无描述符
                if(msg.msg_controllen!=CONTROLLEN){
                    err_dump("status=0 but no fd");
                }
                //遍历消息中控制数据
                for(cmp=CMSG_FIRSTHDR(&msg);cmp!=NULL;cmp=CMSG_NXTHDR(&msg,cmp)){
                    //消息协议非法
                    if(cmp->cmsg_level!=SOL_SOCKET){
                        continue;
                    }
                    //消息分类
                    switch(cmp->cmsg_type){
                    //描述符
                    case SCM_RIGHTS:
                        //保存描述符
                        newfd=*(int*)CMSG_DATA(cmp);
                        //跳出
                        break;
                    //凭证
                    case SCM_CREDTYPE:
                        //保存凭证
                        credp=(struct CREDSTRUCT *)CMSG_DATA(cmp);
                        //保存用户ID
                        *uidptr=credp->CR_UID;
                    }
                }
            }/*if(0==status)*/
            //状态错误
            else{
                newfd=-status;
            }
            //设置消息数量
            nr-=2;
        }//if(*ptr++==0)
    }//for(ptr=buf;ptr<&buf[nr];)
    //如果存在错误消息，则输出
    if(nr>0&&(*userfunc)(STDERR_FILENO,buf,nr)!=nr){
        return(-1);
    }
    //返回
    if(status>=0){
        return(newfd);
    }
}
