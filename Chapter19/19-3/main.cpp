#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#include<grp.h>

#ifndef _HAS_OPENPT

//打开伪终端
int posix_openpt(int oflag){
    int fdm;
    //打开伪终端主设备
    fdm=open("/dev/ptmx",oflag);
    //返回主设备文件描述符
    return(fdm);
}

#endif

#ifndef _HAS_OPENPT

//根据文件描述符获取从设备文件名称
char *ptsname(int fdm){
    int sminor;
    static char pts_name[16];
    //获取相应伪终端从设备
    if(ioctl(fdm,TIOCGPTN,&sminor)<0){
        return(NULL);
    }
    //形成伪终端从设备名称
    snprintf(pts_name,sizeof(pts_name),"/dev/pts/%d",sminor);
    //返回伪终端从设备名称
    return(pts_name);
}

#endif

#ifndef _HAS_OPENPT

//设备伪终端从设备访问权限
int grantpt(int fdm){
    char *pts_name;
    pts_name=ptsname(fdm);
    return(chmod(pts_name,S_IRUSR|S_IWUSR|S_IWGRP));
}

#endif

#ifndef _HAS_OPENPT

//解锁伪终端从设备，使其可用
int unlockpt(int fdm){
    int lock=0;
    return(ioctl(fdm,TIOCSPTLCK,&lock));
}

#endif

//打开伪终端主设备
int ptym_open(char *pts_name,int pts_namesz){
    char *ptr;
    int fdm;
    /**
     * Return the name of the master device so that on failure
     * the caller can print an error message.Null terminate to 
     * handle case where string length > pts_namesz.
     */
    //拷贝伪终端主设备名称
    strncpy(pts_name,"/dev/ptmx",pts_namesz);
    //结束字符
    pts_name[pts_namesz-1]='\0'; // 防止字符串溢出
    //获取第一个未被使用的PTY主设备
    if((fdm=posix_openpt(O_RDWR))<0){
        return(-1);
    }
    //设置从设备的访问权
    if(grantpt(fdm)<0){ // grant access to slave
        close(fdm);
        return(-2);
    }
    //清除从设备锁
    if(unlockpt(fdm)<0){ // clear slave's lock flag
        close(fdm);
        return(-3);
    }
    //获取从设备名称
    if((ptr=ptsname(fdm))==NULL){// get slave's name
        close(fdm);
        return(-4);
    }
    /**
     * Return name of slave. Null terminate to handle
     * case where strlen(ptr) > pts_namesz.
     */
    //返回从设备名称
    strncpy(pts_name,ptr,pts_namesz);
    pts_name[pts_namesz-1]='\0'; // 防止字符串溢出
    //返回主设备文件描述符
    return(fdm);
}

//打开指定伪终端从设备
int ptys_open(char *pts_name){
    int fds;
    if((fds=open(pts_name,O_RDWR))<0){
        return(-5);
    }
    return(fds);
}