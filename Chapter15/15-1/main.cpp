#include"../../apue.h"

int main(void){
    //读取字节数
    int n;
    //管道描述符
    int fd[2];
    //进程ID
    pid_t pid;
    //一行字符
    char line[MAXLINE];
    //创建管道
    if(pipe(fd)<0){
        //创建失败
        err_sys("pipe error");
    }
    //创建子进程
    if((pid=fork())<0){
        //创建失败
        err_sys("fork error");
    }
    //父进程
    else if(pid>0){
        //关闭'读'
        close(fd[0]);
        //写管道
        write(fd[1],"hello world\n",12);
    }
    //子进程
    else{
        //关闭'写'
        close(fd[1]);
        //读管道
        n=read(fd[0],line,MAXLINE);
        //输出到标准输出
        write(STDOUT_FILENO,line,n);
    }
    //退出
    exit(0);
}
