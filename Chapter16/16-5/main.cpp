#include"../16-3/initserver.h"
#include<netdb.h>
#include<errno.h>
#include<syslog.h>
#include<sys/socket.h>

#define BUFLEN 128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

// 实现服务器套接字处理
void serve(int sockfd){
    // 客户端套接字
    int clfd;
    // 管道文件描述符
    FILE *fp;
    // 缓冲区
    char buf[BUFLEN];
    // 循环处理套接字
    for(;;){
        // 等待套接字连接
        clfd=accept(sockfd,NULL,NULL);
        // 如果等待失败
        if(clfd<0){
            // 书写日志
            syslog(LOG_ERR,"ruptimed: accept error: %s",
                strerror(errno));
            // 退出
            exit(1);
        }
        // 创建管道,子进程、执行命令，并将执行结果返回到fp中，失败
        if((fp=popen("/usr/bin/uptime","r"))==NULL){
            // 形成错误描述
            sprintf(buf,"error: %s\n",strerror(errno));
            // 发送错误描述
            send(clfd,buf,strlen(buf),0);
        }
        // 创建管道、子进程执行命令，并将执行结果返回到fp中，成功
        else{
            // 从管道中循环读取执行结果
            while(fgets(buf,BUFLEN,fp)!=NULL){
                // 发送执行结果
                send(clfd,buf,strlen(buf),0);
            }
            // 关闭文件描述符
            pclose(fp);
        }
        // 关闭套接字
        close(clfd);
    }
}

int main(int argc,char * /*argv*/ []){
    struct addrinfo *ailist,*aip;
    struct addrinfo hint;
    int sockfd,err,n;
    char *host;
    // 参数非法
    if(argc!=1){
        err_quit("usage: ruptimed");
    }
    // 换算主机名称最大长度
#ifdef _SC_HOST_NAME_MAX
    n=sysconf(_SC_HOST_NAME_MAX);
    if(n<0)
#endif
      n=HOST_NAME_MAX;
    // 主机缓冲区
    host=(char*)malloc(n);
    // 分配失败
    if(host==NULL){
        err_sys("malloc error");
    }
    // 获取主机名称
    if(gethostname(host,n)<0){
        err_sys("gethostname error");
    }
    //
    //daemonize("ruptimed");
    // 过滤模板
    hint.ai_flags=AI_CANONNAME;
    hint.ai_family=0;
    hint.ai_socktype=0;
    hint.ai_protocol=0;
    hint.ai_addrlen=0;
    hint.ai_canonname=NULL;
    hint.ai_addr=NULL;
    hint.ai_next=NULL;
    // 利用主机名称、服务名称获取相关地址列表
    if((err=getaddrinfo(host,"ruptime",&hint,&ailist))!=0){
        // 错误日志
        syslog(LOG_ERR,"ruptimed: getaddrinfo error: %s",gai_strerror(err));
        // 退出
        exit(1);
    }
    // 遍历地址列表创建服务
    for(aip=ailist;aip!=NULL;aip=aip->ai_next){
        // 初始化套接字
        if((sockfd=initserver(SOCK_STREAM,aip->ai_addr,
            aip->ai_addrlen,QLEN))>=0){
            // 等待接受套接字
            serve(sockfd);
            // 退出程序
            exit(0);
        }
    }
    // 退出
    exit(1);
}