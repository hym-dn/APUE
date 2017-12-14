#include"../../apue.h"
#include<stropts.h>

int send_fd(int fd,int fd_to_send){
    //协议位
    char buf[2];
    //设置协议位
    buf[0]=0;
    //描述符非法
    if(fd_to_send<0){
        //设置错误标识
        buf[1]=-fd_to_send;
        if(0==buf[1]){
            buf[1]=1;
        }
    }
    //描述符合法
    else{
        buf[1]=0;
    }
    //发送协议位
    if(write(fd,buf,2)!=2){
        return(-1);
    }
    /**
     * I_SENDFD
     * Creates a new reference to the open file 
     * description associated with the file descriptor arg, 
     * and writes a message on the STREAMS-based pipe fildes 
     * containing this reference, together with the user ID 
     * and group ID of the calling process.
     */
    if(fd_to_send>=0){
        if(ioctl(fd,I_SENDFD,fd_to_send)<0){
            return(-1);
        }
    }
    //返回
    return(0);
}
