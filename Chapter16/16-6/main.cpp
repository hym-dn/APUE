#include"../../apue.h"
#include<netdb.h>
#include<errno.h>
#include<syslog.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/wait.h>

#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int,const struct sockaddr *,socklen_t,int);

//实现服务器套接字处理
void serve(int sockfd){
    //套接字
    int clfd;
    //子进程退出状态
    int status;
    //进程ID
    pid_t pid;
    //循环处理套接字
    for(;;){
        //等待套接字连接
        clfd=accept(sockfd,NULL,NULL);
        //如果等待失败
        if(clfd<0){
            //书写日志
            syslog(LOG_ERR,"ruptimed: accept error: %s",
                strerror(errno));
            //退出
            exit(1);
        }
        //创建子进程
        if((pid=fork())<0){
            //书写日志
            syslog(LOG_ERR,"ruptimed: accept error: %s",
                strerror(errno));
            //退出
            exit(1);
        }
        //子进程
        else if(pid==0){
            //设置套接子为标准输出、标准出错
            if(dup2(clfd,STDOUT_FILENO)!=STDOUT_FILENO||
               dup2(clfd,STDERR_FILENO)!=STDERR_FILENO){
                //错误日志
                syslog(LOG_ERR,"ruptimed: unexpected error");
                //退出
                exit(1);
            }
            //关闭套接字
            close(clfd);
            //执行命令
            execl("/usr/bin/uptime","uptime",(char*)0);
            //错误日志
            syslog(LOG_ERR,"ruptimed: unexpected return form exec: %s",
                strerror(errno));
        }
        //父进程
        else{
            //关闭描述符
            close(clfd);
            //等待子进程结束
            waitpid(pid,&status,0);
        }
    }
}

int main(int argc,char * /*argv*/ []){
    struct addrinfo *ailist,*aip;
    struct addrinfo hint;
    int sockfd,err,n;
    char *host;
    //参数非法
    if(argc!=1){
        err_quit("usage: ruptimed");
    }
    //换算主机名称最大长度
#ifdef _SC_HOST_NAME_MAX
    n=sysconf(_SC_HOST_NAME_MAX);
    if(n<0)
#endif
      n=HOST_NAME_MAX;
    //主机缓冲区
    host=(char*)malloc(n);
    //分配失败
    if(host==NULL){
        err_sys("malloc error");
    }
    //获取主机名称
    if(gethostname(host,n)<0){
        err_sys("gethostname error");
    }
    //
    //daemonize("ruptimed");
    //过滤模板
    hint.ai_flags=AI_CANONNAME;
    hint.ai_family=0;
    hint.ai_socktype=0;
    hint.ai_protocol=0;
    hint.ai_addrlen=0;
    hint.ai_canonname=NULL;
    hint.ai_addr=NULL;
    hint.ai_next=NULL;
    //利用主机名称、服务名称获取相关地址列表
    if((err=getaddrinfo(host,"ruptime",&hint,&ailist))!=0){
        //错误日志
        syslog(LOG_ERR,"ruptimed: getaddrinfo error: %s",gai_strerror(err));
        //退出
        exit(1);
    }
    //遍历地址列表创建服务
    for(aip=ailist;aip!=NULL;aip=aip->ai_next){
        //初始化套接字
        if((sockfd=initserver(SOCK_STREAM,aip->ai_addr,
            aip->ai_addrlen,QLEN))>=0){
            //等待接受套接字
            serve(sockfd);
            //退出程序
            exit(0);
        }
    }
    //退出
    exit(1);
}


/**
 * type - 套接子类型。
 * addr - 地址信息。
 * alen - 地址信息尺寸。
 * qlen - 允许的连接数。
 */ 
int initserver(int type,const struct sockaddr *addr,socklen_t alen,int qlen){
    //记录套接字
    int fd;
    //记录错误
    int err=0;
    //创建一个套接子
    if((fd=socket(addr->sa_family,type,0))<0){
        return(-1);
    }
    //绑定地址
    if(bind(fd,addr,alen)<0){
        err=errno;
        goto errout;
    }
    //如果套接字面向连接
    if(SOCK_STREAM==type||SOCK_SEQPACKET==type){
        //监听套接字
        if(listen(fd,qlen)<0){
            err=errno;
            goto errout;
        }
    }
    //返回套接字
    return(fd);
//错误处理
errout:
    close(fd);
    errno=err;
    return(-1);
}
