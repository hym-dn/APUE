#include"open.h"
#include<sys/uio.h>

int csopen(char *name,int oflag){
    pid_t pid;
    int len;
    char buf[10];
    struct iovec iov[3];
    static int fd[2]={-1,-1};
    //如果文件描述符尚未打开
    if(fd[0]<0){
        //创建相互连接的管道
        if(s_pipe(fd)<0){
            //出错
            err_ret("s_pipe error");
            //返回
            return(-1);
        }
        //创建子进程
        if((pid=fork())<0){
            //出错
            err_ret("fork error");
            //返回
            return(-1);
        }
        //子进程
        else if(0==pid){
            //关闭读管道
            close(fd[0]);
            //重新定位写管道到为标准输入
            if(fd[1]!=STDIN_FILENO&&
                dup2(fd[1],STDIN_FILENO)!=STDIN_FILENO){
                err_sys("dup2 error to stdin");
            }
            //重新定位写管道为标准输出
            if(fd[1]!=STDOUT_FILENO&&
               dup2(fd[1],STDOUT_FILENO)!=STDOUT_FILENO){
                err_sys("dup2 error to stdout");
            }
            //执行服务器
            if(execl("./opend","opend",(char *)0)<0){
                err_sys("execl error");
            }
        }
        //父进程
        //关闭写管道
        close(fd[1]);
    }
    //形成文件打开标志
    sprintf(buf," %d",oflag);
    //设置发送数据
    iov[0].iov_base=CL_OPEN " ";
    iov[0].iov_len=strlen(CL_OPEN)+1;
    iov[1].iov_base=name;
    iov[1].iov_len=strlen(name);
    iov[2].iov_base=buf;
    iov[2].iov_len=strlen(buf)+1;
    len=iov[0].iov_len+iov[1].iov_len+iov[2].iov_len;
    //通过管道发送数据
    if(writev(fd[0],&iov[0],3)!=len){
        //出错
        err_ret("writev error");
        //返回
        return(-1);
    }
    //读取文件描述符
    return(recv_fd(fd[0],write));
}
