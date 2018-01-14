#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#include<stropts.h>

//打开伪终端主设备
int ptym_open(char *pts_name,int pts_namesz){
    char *ptr;
    int fdm;
    /**
     * Return the name of the master device so that on failure
     * the caller can print an error message.Null terminate to 
     * handle case where strlen("/dev/ptmx") > pts_namesz.
     */
    //拷贝STREAMS克隆设备（clone device）名称
    strncpy(pts_name,"/dev/ptmx",pts_namesz);
    //结束字符
    pts_name[pts_namesz-1]='\0'; // 防止字符串溢出
    //获取第一个未被使用的PTY主设备
    if((fdm=open(pts_name,O_RDWR))<0){
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

//打开从设备
int ptys_open(char *pts_name){
    int fds,setup;
    /**
     * The following open should allocate a 
     * controlling terminal.
     */
    //打开从设备，并将从设备设置为当前进程控制终端
    //如果不想将设备设置为当前进程控制终端，则需要
    //增加O_NOTTY标志
    if((fds=open(pts_name,O_RDWR))<0){
        return(-5);
    }
    //Check if stream is already set up by autopush
    //facility
    //检测ldterm是否被自动压入
    if((setup=ioctl(fds,I_FIND,"ldterm"))<0){
        close(fds);
        return(-6);
    }
    //如果ldterm并非自动压入
    if(setup==0){
        //压入ptem
        if(ioctl(fds,I_PUSH,"ptem")<0){
            close(fds);
            return(-7);
        }
        //压入ldterm
        if(ioctl(fds,I_PUSH,"ldterm")<0){
            close(fds);
            return(-8);
        }
        //压入ttcompat
        if(ioctl(fds,I_PUSH,"ttcompat")<0){
            close(fds);
            return(-9);
        }
    }
    //返回从设备文件描述符
    return(fds);
}