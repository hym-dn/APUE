#include"../../apue.h"

// 从指定的描述符中读取n个字节
ssize_t readn(int fd,void *ptr,size_t n){ /* Read "n" bytes from a descriptor */
    //
    size_t nleft;
    ssize_t nread;
    // 余下未写的字节
    nleft=n;
    // 余下未写的字节存在
    while(nleft>0){
        // 读取指定描述符
        if((nread=read(fd,ptr,nleft))<0){ // 读取失败
            // 尚未读取任何字节
            if(nleft==n){
                return(-1);
            }
            // 已经读取部分字节
            else{
                break;
            }
        }
        // 读取到文件尾部
        else if(nread==0){ // 读取文件尾
            break; /* EOF */
        }
        // 重置缓冲区
        nleft-=nread;
        ptr+=nread;
    }
    // 返回
    return(n-nleft);
}

// 向指定的描述符写n个字节
ssize_t writen(int fd,const void *ptr,size_t n){ /* Write "n" bytes to a descriptor */
    size_t nleft;
    ssize_t nwritten;
    // 余下字节
    nleft=n;
    // 余下字节不存在
    while(nleft>0){
        // 向指定的文件描述符写入字节
        if((nwritten=write(fd,ptr,nleft))<0){ // 写失败
            // 如果尚未写入任何字节
            if(nleft==n){
                return(-1); /* error,return -1 */
            }
            // 写入部分字节
            else{
                break; /* error,return amount written so far */
            }
        }
        // 如果写入字节为0
        else if(nwritten==0){ // 满或者出错
            break;
        }
        // 重置缓冲区
        nleft-=nwritten;
        ptr+=nwritten;
    }
    // 返回
    return(n-nleft); /* return >= 0 */
}