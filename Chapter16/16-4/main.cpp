#include"../16-2/connect_retry.h"
#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>

#define MAXADDRLEN 256
#define BUFLEN 128
#define MAXSLEEP 128

// 打印读取的时间
void print_uptime(int sockfd){
    int n;
    char buf[BUFLEN];
    // 读指定套接字，直到读完或出错
    while((n=recv(sockfd,buf,BUFLEN,0))>0){
        // 输出读取内容
        write(STDOUT_FILENO,buf,n);
    }
    // 如果读失败
    if(n<0){
        err_sys("recv error");
    }
}

int main(int argc,char *argv[]){
    // 映射地址
    struct addrinfo *ailist,*aip;
    // 过滤模板
    struct addrinfo hint;
    // 套接字、错误
    int sockfd,err;
    // 如果参数非法
    if(argc!=2){
        err_quit("usage: ruptime hostname");
    }
    // 设置过滤模板
    hint.ai_flags=0;
    hint.ai_family=0;
    hint.ai_socktype=SOCK_STREAM;
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
        // 创建套接字
        if((sockfd=socket(aip->ai_family,SOCK_STREAM,0))<0){
            err=errno;
        }
        // 连接失败
        if(connect_retry(sockfd,aip->ai_addr,aip->ai_addrlen)<0){
            err=errno;
        }
        // 连接成功
        else{
            // 打印时间
            print_uptime(sockfd);
            // 退出
            exit(0);
        }
    }
    // 打印错误
    fprintf(stderr,"can't connect to %s: %s\n",argv[1],strerror(err));
    // 退出
    exit(1);
}