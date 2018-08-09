#include"../../apue.h"

// 信号捕捉函数
static void sig_pipe(int); /* our signal handler */

int main(void){
    // 数量,管道
    int n,fd1[2],fd2[2];
    // 进程ID
    pid_t pid;
    // 行缓冲区
    char line[MAXLINE];
    // 捕捉信号
    if(signal(SIGPIPE,sig_pipe)==SIG_ERR){
        err_sys("signal error");
    }
    // 创建管道
    if(pipe(fd1)<0||pipe(fd2)<0){ // 失败
        err_sys("pipe error");
    }
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 父进程
    else if(pid>0){ /* parent */
        // 关闭读
        close(fd1[0]);
        // 关闭写
        close(fd2[1]);
        // 读标准输入
        while(fgets(line,MAXLINE,stdin)!=NULL){
            // 读取长度
            n=strlen(line);
            // 写管道
            if(write(fd1[1],line,n)!=n){
                err_sys("write error to pipe");
            }
            // 读管道
            if((n=read(fd2[0],line,MAXLINE))<0){
                err_sys("read error from pipe");
            }
            // 管道关闭
            if(0==n){
                err_msg("child closed pipe");
                break;
            }
            // 添加结束字符
            line[n]=0;
            // 输出
            if(fputs(line,stdout)==EOF){
                err_sys("fputs error");
            }
        }
        // 异常检测
        if(ferror(stdin)){
            err_sys("fgets error on stdin");
        }
        // 退出
        exit(0);
    }
    // 子进程
    else{ /* child */
        // 关闭写
        close(fd1[1]);
        // 关闭读
        close(fd2[0]);
        //绑定标准输入
        if(fd1[0]!=STDIN_FILENO){
            //绑定
            if(dup2(fd1[0],STDIN_FILENO)!=STDIN_FILENO){
                err_sys("dup2 error to stdin");
            }
            //关闭原读
            close(fd1[0]);
        }
        //绑定标准输出
        if(fd2[1]!=STDOUT_FILENO){
            //绑定
            if(dup2(fd2[1],STDOUT_FILENO)!=STDOUT_FILENO){
                err_sys("dup2 error to stdout");
            }
            //关闭原写
            close(fd2[0]);
        }
        //执行
        if(execl("./add2","add2",(char*)0)<0){
            err_sys("execl error");
        }
        //退出
        exit(0);
    }
}

//信号捕获函数
static void sig_pipe(int signo){
    printf("SIGPIPE caught\n");
    exit(1);
}
