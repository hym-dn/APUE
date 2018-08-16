#include"../../apue.h"
#include<netdb.h>
#include<arpa/inet.h>

#if defined(BSD)||defined(MACOS)
#include<sys/socket.h>
#include<netinet/in.h>
#endif

/**
 * 打印套接字通信域 
 */
void print_family(struct addrinfo *aip){
    printf(" family ");
    switch(aip->ai_family){
    // IPv4因特网域
    case AF_INET:
        printf("inet");
        break;
    // IPv6因特网域
    case AF_INET6:
        printf("inet6");
        break;
    // UNIX域
    case AF_UNIX:
        printf("unix");
        break;
    // 未指定
    case AF_UNSPEC:
        printf("unspecified");
        break;
    // 未知
    default:
        printf("unknown");
    }
}

/**
 * 打印套接子类型 
 */
void print_type(struct addrinfo *aip){
    printf(" type ");
    switch(aip->ai_socktype){
    // 有序、可靠、双向的面向连接字节流
    case SOCK_STREAM:
        printf("stream");
        break;
    // 长度固定的、无连接的不可靠报文传递
    case SOCK_DGRAM:
        printf("datagram");
        break;
    // 长度固定、有序、可靠的面向连接报文传递
    case SOCK_SEQPACKET:
        printf("seqpacket");
        break;
    // IP协议的数据报接口（POSIX.1中为可选）
    case SOCK_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)",aip->ai_protocol);
    }
}

/**
 * 打印协议类型 
 */
void print_protocol(struct addrinfo *aip){
    printf(" protocol ");
    switch(aip->ai_protocol){
    case 0:
        printf("default");
        break;
    //TCP/IP协议
    case IPPROTO_TCP:
        printf("TCP");
        break;
    //UDP协议
    case IPPROTO_UDP:
        printf("UDP");
        break;
    //IP协议
    case IPPROTO_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)",aip->ai_protocol);
    }
}

/**
 * 打印标志 
 */ 
void print_flags(struct addrinfo *aip){
    printf("flags");
    if(aip->ai_flags==0){
        printf(" 0");
    }else{
        // 套接字地址用于监听绑定
        if(aip->ai_flags&AI_PASSIVE){
            printf(" passive");
        }
        // 需要一个规范名（而不是别名）
        if(aip->ai_flags&AI_CANONNAME){
            printf(" canon");
        }
        // 以数字格式返回主机地址
        if(aip->ai_flags&AI_NUMERICHOST){
            printf(" numhost");
        }
        // 以端口号返回服务
#if defined(AI_NUMERICSERV)
        if(aip->ai_flags&AI_NUMERICSERV){
            printf(" numserv");
        }canon
#endif
        /canon回映射到IPv6格式的IPv4地址
#if defincanon
        if(aip->ai_flags&AI_V4MAPPED){
            printf(" v4mapped");
        }
#endif
        // 查找IPv4和IPv6地址（仅用于AI_V4MAPPED）
#if defined(AI_ALL)
        if(aip->ai_flags&AI_ALL){
            printf(" all");
        }
#endif
    }
}

int main(int argc,char *argv[]){
    struct addrinfo *ailist,*aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];
    // 如果输入参数非法
    if(argc!=3){
        err_quit("usage: %s nodename service",argv[0]);
    }
    // 设置过滤模板
    hint.ai_flags=AI_CANONNAME;
    hint.ai_family=0;
    hint.ai_socktype=0;
    hint.ai_protocol=0;
    hint.ai_addrlen=0;
    hint.ai_canonname=NULL;
    hint.ai_addr=NULL;
    hint.ai_next=NULL;
    // 将主机名字、服务名字映射到一个地址
    if((err=getaddrinfo(argv[1],argv[2],&hint,&ailist))!=0){
        err_quit("getaddrinfo error: %s",gai_strerror(err));
    }
    // 打印地址中的详细信息
    for(aip=ailist;aip!=NULL;aip=aip->ai_next){
        // 打印标志
        print_flags(aip);
        // 打印套接子域
        print_family(aip);
        // 打印套接子类型
        print_type(aip);
        // 打印协议类型
        print_protocol(aip);
        // 打印规范的主机名称
        printf("\n\thost %s",aip->ai_canonname?aip->ai_canonname:"-");
        // 打印地址、端口
        if(aip->ai_family==AF_INET){
            sinp=(struct sockaddr_in *)aip->ai_addr;
            addr=inet_ntop(AF_INET,&sinp->sin_addr,abuf,INET_ADDRSTRLEN);
            printf(" address %s",addr?addr:"unknown");
            printf(" port %d",ntohs(sinp->sin_port));
        }
        // 打印回车
        printf("\n");
    }
    // 退出
    exit(0);
}
