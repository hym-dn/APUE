#include"../../apue.h"
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

#define CLI_PATH "/var/tmp/"
//用户读写执行
#define CLI_PERM S_IRWXU

/**
 * 创建一个连接到指定服务的套接字
 * 如果函数执行成功返回套接字描述符，
 * 否则返回值小于0
 */
int cli_conn(const char *name){
    int fd,len,err,rval;
    struct sockaddr_un un,sun;
    int do_unlink=0;
    //名字过长
    if(strlen(name)>=sizeof(un.sun_path)){
        errno=ENAMETOOLONG;
        return(-1);
    }
    //创建UNIX阈套套接字
    if((fd=socket(AF_UNIX,SOCK_STREAM,0))<0){
        return(-1);
    }
    //设置套接字地址
    memset(&un,0,sizeof(un));
    un.sun_family=AF_UNIX;
    sprintf(un.sun_path,"%s%05ld",CLI_PATH,(long)getpid());
    len=offsetof(struct sockaddr_un,sun_path)+strlen(un.sun_path);
    //减少引用计数
    unlink(un.sun_path);
    //绑定地址
    if(bind(fd,(struct sockaddr *)&un,len)<0){
        rval=-2;
        goto errout;
    }
    //文件权限
    if(chmod(un.sun_path,CLI_PERM)<0){
        rval=-3;
        do_unlink=1;
        goto errout;
    }
    //设置地址
    memset(&sun,0,sizeof(sun));
    sun.sun_family=AF_UNIX;
    strcpy(sun.sun_path,name);
    len=offsetof(struct sockaddr_un,sun_path)+strlen(name);
    //连接
    if(connect(fd,(struct sockaddr *)&sun,len)<0){
        rval=-4;
        do_unlink=1;
        goto errout;
    }
    //返回
    return(fd);
//错误处理
errout:
    err=errno;
    close(fd);
    if(do_unlink){
        unlink(un.sun_path);
    }
    errno=err;
    return(rval);
}

