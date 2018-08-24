#include"../17-18/open.h"
#include"../17-2/s_pipe.h"
#include"../17-13/recv_fd.h"
#include<sys/uio.h> /* struct iovec */

// 通过发送名称、标志给连接的服务器，并且返回一个文件描述符
/**
 * Open the file by sending the "name" and "oflag" to the
 * connection server and reading a file descriptor back.
 */
int csopen(char *name,int oflag){
    pid_t pid;
    int len;
    char buf[10];
    struct iovec iov[3];
    // s管道
    static int fd[2]={-1,-1};
    // 如果管道尚未打开
    if(fd[0]<0){ /* fork/exec our open server first time */
        // 创建s管道
        if(s_pipe(fd)<0){
            // 出错
            err_ret("s_pipe error");
            // 返回
            return(-1);
        }
        // 创建进程
        if((pid=fork())<0){
            // 出错
            err_ret("fork error");
            // 返回
            return(-1);
        }
        // 子进程
        else if(0==pid){ /* child */
            // 关闭读管道
            close(fd[0]);
            // 重新定位写管道到为标准输入
            if(fd[1]!=STDIN_FILENO&&dup2(
                fd[1],STDIN_FILENO)!=STDIN_FILENO){
                err_sys("dup2 error to stdin");
            }
            // 重新定位写管道为标准输出
            if(fd[1]!=STDOUT_FILENO&&dup2(
                fd[1],STDOUT_FILENO)!=STDOUT_FILENO){
                err_sys("dup2 error to stdout");
            }
            // 执行服务器
            if(execl("./opend","opend",(char *)0)<0){
                err_sys("execl error");
            }
        }
        // 关闭写管道
        close(fd[1]); /* parent */
    }
    // 以ascii形式传递打开标志
    sprintf(buf," %d",oflag); /* oflag to ascii */
    // 设置发送数据
    iov[0].iov_base=(void*)(CL_OPEN " "); /* string concatenation */
    iov[0].iov_len=strlen(CL_OPEN)+1;
    iov[1].iov_base=name;
    iov[1].iov_len=strlen(name);
    iov[2].iov_base=buf;
    iov[2].iov_len=strlen(buf)+1; /* +1 for null at endof buf */
    // 计算最终数据长度
    len=iov[0].iov_len+iov[1].iov_len+iov[2].iov_len;
    // 将打开命令发送给服务器
    if(writev(fd[0],&iov[0],3)!=len){
        // 出错
        err_ret("writev error");
        // 返回
        return(-1);
    }
    // 读取打开的文件描述符
    /* read descriptor,returned errors handled by write() */
    return(recv_fd(fd[0],write));
}