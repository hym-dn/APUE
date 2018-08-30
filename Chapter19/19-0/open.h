#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#if defined(SLOARIS)
#include<stropts.h>
#endif

/**
 * 打开伪终端主设备
 * pts_name - 返回的伪终端从设备名称
 * pts_namesz - 伪终端从设备名称尺寸
 */
int ptym_open(char *pts_name,int pts_namesz){
    char *ptr;
    int fdm,err;
    // 打开伪终端主设备
    if((fdm=posix_openpt(O_RDWR))<0){
        return(-1);
    }
    // 设置伪终端主设备相应从设备的访问权限
    if(grantpt(fdm)<0){ /* grant access to slave */
        goto errout;
    }
    // 解锁从设备访问限制
    if(unlockpt(fdm)<0){ /* clear slave's lock flag */
        goto errout;
    }
    // 获取从设备名称
    if((ptr=ptsname(fdm))==NULL){ /* get slave's name */
        goto errout;
    }
    /**
     * Return name of slave. Null terminate to handle
     * case where strlen(ptr) > pts_namesz.
     */
    strncpy(pts_name,ptr,pts_namesz);
    pts_name[pts_namesz-1]='\0';
    // 返回
    return(fdm); /* return fd of master */
// 错误处理
errout:
    err=errno;
    close(fdm);
    errno=err;
    return(-1);
}

/**
 * 打开伪终端从设备
 * pts_name - 输入的伪终端从设备名称
 */
int ptys_open(char *pts_name){
    int fds;
#if defined(SLOARIS)
    int err,setup;
#endif
    // 打开伪终端从设备
    if((fds=open(pts_name,O_RDWR))<0){
        return(-1);
    }
#if defined(SLOARIS)
    /**
     * Check if stream is already set up by autopush facility.
     */
    // 检测流“ldterm”是否已被设置
    if((setup=ioctl(fds,I_FIND,"ldterm"))<0){
        goto errout;
    }
    // 如果“ldterm”尚未被设置
    if(setup==0){
        if(ioctl(fds,I_PUSH,"ptem")<0){
            goto errout;
        }
        if(ioctl(fds,I_PUSH,"ldterm")<0){
            goto errout;
        }
        if(ioctl(fds,I_PUSH,"ttcompat")<0){
errout:
            err=errno;
            close(fds);
            errno=err;
            return(-1);
        }
    }
#endif
    // 返回伪终端从设备文件描述符
    return(fds);
}