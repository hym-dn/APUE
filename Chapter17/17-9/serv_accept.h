#include"../../apue.h"
#include<sys/socket.h>
#include<sys/un.h>
#include<time.h>
#include<errno.h>

#define STALE 30 /* client's name can't be older than this (sec) */

/**
 * 等待客户端连接。
 * 如果成功返回新套接子，否则返回小于0.
 */
/**
 * Wait for a client connection to arrive, and accept it.
 * We also obtain the client's user ID from the pathname
 * that it must bind before calling us.
 * Returns new fd if all OK, <0 on error 
 */
int serv_accept(int listenfd,uid_t *uidptr){
    int clifd,err,rval;
    socklen_t len;
    time_t staletime;
    struct sockaddr_un un;
    struct stat statbuf;
    char *name;
    // 分配套接字名称缓冲区
    // allocate enough space for longest name plus terminating null
    if((name=(char *)malloc(sizeof(un.sun_path)+1))==NULL){
        return(-1);
    }
    // 计算地址尺寸
    len=sizeof(un);
    // 接收套接字
    if((clifd=accept(listenfd,(struct sockaddr *)&un,&len))<0){
        free(name);
        return(-2); /* often errno=EINTR,if signal caught */
    }
    // 路径长度
    len-=offsetof(struct sockaddr_un,sun_path);
    // 拷贝路径
    memcpy(name,un.sun_path,len);
    // 追加结束符
    name[len]=0;
    // 获取文件属性
    if(stat(name,&statbuf)<0){
        rval=-3;
        goto errout;
    }
    // 检测文件是否为套接字
#ifdef S_ISSOCK /* not defined for SVR4 */
    if(S_ISSOCK(statbuf.st_mode)==0){
        rval=-4; /* not a socket */
        goto errout;
    }
#endif
    // 文件不是rwx------
    if((statbuf.st_mode&(S_IRWXG|S_IRWXO))||
       (statbuf.st_mode&S_IRWXU)!=S_IRWXU){
        rval=-5; /* is not rwx------ */
        goto errout;
    }
    // 计算文件创建时间
    staletime=time(NULL)-STALE;
    // 如果文件过老
    if(statbuf.st_atime<staletime||
       statbuf.st_ctime<staletime||
       statbuf.st_mtime<staletime){
       rval=-6; /* i-node is too old */
       goto errout;
    }
    // 返回用户ID
    if(uidptr!=NULL){
        *uidptr=statbuf.st_uid; /* return uid of caller */
    }
    // 解除链接次数
    unlink(name); /* we're done with pathname now */
    // 释放内存
    free(name);
    // 返回
    return(clifd);
// 错误
errout:
    err=errno;
    close(clifd);
    free(name);
    errno=err;
    return(rval);
}