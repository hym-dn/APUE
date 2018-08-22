#include"../../apue.h"
#include<fcntl.h>
#include<stropts.h>

/**
 * 管道文件允许用户读写、组读写、其他读写权限
 */
/* pipe permissions: user rw, group rw, others rw */
#define FIFO_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/**
 * 监听客户端管道连接请求。
 * name - 文件名称。
 * 如果函数执行成功返回管道文件描述符，否则返回值小于0。
 */
/**
 * Establish an endpoint to listen for connect requests.
 * returns fd is all Ok,<0 on error 
 */
int serv_listen(const char *name){
    // 临时文件描述符
    int tempfd;
    // 管道文件描述符
    int fd[2];
    // 取消文件链接
    /**
     * Create a file: mount point for fattach().
     */
    unlink(name);
    // 创建相应文件
    if((tempfd=creat(name,FIFO_MODE))<0){
        // 返回错误
        return(-1);
    }
    // 关闭临时文件(自动删除)
    if(close(tempfd)<0){
        // 返回错误
        return(-2);
    }
    // 创建管道
    if(pipe(fd)<0){
        // 返回错误
        return(-3);
    }
    // 将"connld"模块推送到流中
    if(ioctl(fd[1],I_PUSH,"connld")<0){
        // 关闭读管道
        close(fd[0]);
        // 关闭写管道
        close(fd[1]);
        // 返回错误
        return(-4);
    }
    // 为管道分配文件
    /**
     * Push connld & fattach() on fd[1].
     */
    if(fattach(fd[1],name)<0){
        // 关闭读管道
        close(fd[0]);
        // 关闭写管道
        close(fd[1]);
        // 返回错误
        return(-5);
    }
    // 关闭写管道
    close(fd[1]); /* fattach holds this end open */
    // 返回读管道
    return(fd[0]); /* fd[0] is where client connections arrive */
}
