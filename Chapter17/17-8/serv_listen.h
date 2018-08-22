#include"../../apue.h"
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

#define QLEN 10

/**
 * 创建监听套接字
 * 如果创建成功返回套接字，否则返回值小于0
 */
/**
 * Create a server endpoint of a connection.
 * Returns fd if all Ok,<0 on error.
 */
int serv_listen(const char *name){
    int fd,len,err,rval;
    // 创建地址
    struct sockaddr_un un;
    // 名称过长
    if(strlen(name)>=sizeof(un.sun_path)){
        errno=ENAMETOOLONG;
        return(-1);
    }
    // 创建UNIX域套接字
    if((fd=socket(AF_UNIX,SOCK_STREAM,0))<0){
        return(-2);
    }
    // 解除链接计数
    unlink(name);
    // 设置地址
    memset(&un,0,sizeof(un));
    un.sun_family=AF_UNIX;
    strcpy(un.sun_path,name);
    len=offsetof(struct sockaddr_un,sun_path)+
        strlen(name);
    // 绑定套接字
    if(bind(fd,(struct sockaddr *)&un,len)<0){
        rval=-3;
        goto errout;
    }
    // 监听套接字
    if(listen(fd,QLEN)<0){
        rval=-4;
        goto errout;
    }
    // 返回
    return(fd);
//错误处理
errout:
    err=errno;
    close(fd);
    errno=err;
    return(rval);
}