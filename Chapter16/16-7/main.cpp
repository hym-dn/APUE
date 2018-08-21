#include"../../apue.h"
#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>

#define BUFLEN 128
#define TIMEOUT 20 // 超时时间

// 报警信号捕获函数
void sigalrm(int /*signo*/){
}

// 打印更新时间
void print_uptime(int sockfd,struct addrinfo *aip){
    int n;
    char buf[BUFLEN];
    buf[0]=0;
    // 向目标地址发送一个0(在无连接的套接子上指定一个目标地址)
    if(sendto(sockfd,buf,1,0,aip->ai_addr,aip->ai_addrlen)<0){
        // 出错退出
        err_sys("sendto error");
    }
    // 启动定时器
    alarm(TIMEOUT);
    // 接收数据
    if((n=recvfrom(sockfd,buf,BUFLEN,0,NULL,NULL))<0){
        // 不是中断
        if(errno!=EINTR){
            alarm(0);
        }
        // 出错退出
        err_sys("recv error");
    }
    // 关闭定时器
    alarm(0);
    // 输出到标准输出
    write(STDOUT_FILENO,buf,n);
}

int main(int argc,char *argv[]){
    // 服务器地址
    struct addrinfo *ailist,*aip;
    // 过滤模板
    struct addrinfo hint;
    // 套接字、错误
    int sockfd,err;
    // 信号
    struct sigaction sa;
    // 如果参数非法
    if(argc!=2){
        err_quit("usage: ruptime hostname");
    }
    // 捕获定时器信号
    sa.sa_handler=sigalrm;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM,&sa,NULL)<0){
        err_sys("sigcation error");
    }
    // 设置过滤模板
    hint.ai_flags=0;
    hint.ai_family=0;
    hint.ai_socktype=SOCK_DGRAM;
    hint.ai_protocol=0;
    hint.ai_addrlen=0;
    hint.ai_canonname=NULL;
    hint.ai_addr=NULL;
    hint.ai_next=NULL;
    // 将主机名、服务映射到一个地址
    if((err=getaddrinfo(argv[1],"ruptime",&hint,&ailist))!=0){
        err_quit("getaddrinfo error: %s",gai_strerror(err));
    }
    // 遍历地址
    for(aip=ailist;aip!=NULL;aip=aip->ai_next){
        // 创建套接字失败
        if((sockfd=socket(aip->ai_family,SOCK_DGRAM,0))<0){
            err=errno;
        }
        // 创建套接字成功
        else{
            // 打印时间
            print_uptime(sockfd,aip);
            // 退出
            exit(0);
        }
    }
    // 打印错误
    fprintf(stderr,"can't connect to %s: %s\n",argv[1],strerror(err));
    // 退出
    exit(1);
}
