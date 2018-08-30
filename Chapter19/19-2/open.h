#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#include<grp.h>

#ifndef _HAS_OPENPT

/**
 * 打开伪终端主设备
 * oflag - 打开标志
 */
int posix_openpt(int oflag){
    int fdm;
    char *ptr1,*ptr2;
    char ptm_name[16];
    // 设置主设备前缀
    strcpy(ptm_name,"/dev/ptyXY");
    // array index: 0123456789 (for references in following code)
    // 查找可用的伪终端主设备，并打开
    for(ptr1="pqrstuvwxyzPQRST";*ptr1!=0;ptr1++){
        // 第一部分伪终端主设备名称
        ptm_name[8]=*ptr1;
        // 第二部分伪终端主设备名称
        for(ptr2="0123456789abcdef";*ptr2!=0;ptr2++){
            // 第二部分伪终端主设备名称
            ptm_name[9]=*ptr2;
            /**
             * Try to open the master.
             */
            // 尝试打开主设备
            if((fdm=open(ptm_name,oflag))<0){
                // 设备不存在
                if(errno==ENOENT){ /* different from EIO */
                    return(-1); /* out of pty devices */
                }
                // 设备已被使用
                else{
                    continue; /* try next pty device */
                }
            }
            // 返回文件描述符
            return(fdm); /* got it,return fd of master */
        }
    }
    // 未能找到伪终端
    errno=EAGAIN;
    // 返回错误
    return(-1); /* out of pty devices */
}

#endif

#ifndef _HAS_OPENPT

/**
 * 根据主设备文件描述符获取从设备文件名称
 */
char *ptsname(int fdm){
    static char pts_name[16];
    char *ptm_name;
    // 获取主设备文件名称
    ptm_name=ttyname(fdm);
    if(ptm_name==NULL){ // 获取失败
        return(NULL);
    }
    // 拷贝伪终端主设备名称
    strncpy(pts_name,ptm_name,sizeof(pts_name));
    // 追加终止字符
    pts_name[sizeof(pts_name)-1]='\0';
    // 形成从设备文件名称
    if(strncmp(pts_name,"/dev/pty/",9)==0){
        pts_name[9]='s'; // change /dev/pty/mXX to /dev/pty/sXX
    }else{
        pts_name[5]='t'; // change "pty" to "tty"
    }
    // 返回名称
    return(pts_name);
}

#endif

#ifndef _HAS_OPENPT

/**
 * 设置设备伪终端从设备访问权限
 * fdm - 主设备文件描述符
 */
int grantpt(int fdm){
    struct group *grptr; // 组信息结构体
    int gid;
    char *pts_name;
    // 伪终端从设备名称
    pts_name=ptsname(fdm);
    // 获取指定组名称的组信息
    if((grptr=getgrnam("tty"))!=NULL){ // 获取成功
        gid=grptr->gr_gid;
    }else{ // 获取失败
        gid=-1; /* group tty is not in the group file */
    }
    /**
     * The following two calls won't work unless we're the superuser.
     */
    // 更换伪终端从设备用户ID、组ID
    if(chown(pts_name,getuid(),gid)<0){
        return(-1);
    }
    // 更改伪终端访问权限
    return(chmod(pts_name,S_IRUSR|S_IWUSR|S_IWGRP));
}

#endif

#ifndef _HAS_OPENPT

/**
 * 解锁伪终端从设备，使其可用
 */
int unlockpt(int fdm){
    return(0); // nothing to do
}

#endif

/**
 * 打开伪终端主设备
 * pts_name - 伪终端主设备名称（返回的）
 * pts_namesz - 存储伪终端主设备名称缓冲区大小
 * 返回打开的主设备文件描述符
 */
int ptym_open(char *pts_name,int pts_namesz){
    char *ptr;
    int fdm;
    /**
     * Return the name of the master device so that on failure
     * the caller can print an error message.Null terminate to 
     * handle case where string length > pts_namesz.
     */
    // 拷贝伪终端主设备名称
    strncpy(pts_name,"/dev/ptyXX",pts_namesz);
    // 结束字符
    pts_name[pts_namesz-1]='\0'; // 防止字符串溢出
    // 获取第一个未被使用的PTY主设备
    if((fdm=posix_openpt(O_RDWR))<0){
        return(-1);
    }
    // 设置从设备的访问权
    if(grantpt(fdm)<0){ /* grant access to slave */
        close(fdm);
        return(-2);
    }
    // 允许从设备访问
    if(unlockpt(fdm)<0){ /* clear slave's lock flag */
        close(fdm);
        return(-3);
    }
    // 获取从设备名称
    if((ptr=ptsname(fdm))==NULL){ /* get slave's name */
        close(fdm);
        return(-4);
    }
    /**
     * Return name of slave. Null terminate to handle
     * case where strlen(ptr) > pts_namesz.
     */
    // 返回从设备名称
    strncpy(pts_name,ptr,pts_namesz);
    pts_name[pts_namesz-1]='\0'; // 防止字符串溢出
    // 返回主设备文件描述符
    return(fdm);
}

/**
 * 打开从设备
 * pts_name - 输入的从设备名称
 */
int ptys_open(char *pts_name){
    int fds;
    if((fds=open(pts_name,O_RDWR))<0){
        return(-5);
    }
    return(fds);
}