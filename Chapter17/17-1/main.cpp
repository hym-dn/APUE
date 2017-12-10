#include"../../apue.h"

//信号捕获函数
static void sig_pipe(int);

//入口
int main(void){
    int n;
    //文件描述符
    int fd[2];
    //进程ID
    pid_t pid;
    //行缓冲区
    char line[MAXLINE];
    //捕获信号
    if(signal(SIGPIPE,sig_pipe)==SIG_ERR){
        err_sys("signal error");
    }
    //创建流管道
    if(s_pipe(fd)<0){
        err_sys("pipe error");
    }
    //创建进程
    if((pid=fork())<0){
        err_sys("fork error");
    }//创建进程失败
    //父进程
    else if(pid>0){
        //关闭读管道
        close(fd[1]);
        //标准输入中读取
        while(fgets(line,MAXLINE,stdin)!=NULL){
            //读入长度
            n=strlen(line);
            //写管道
            if(write(fd[0],line,n)!=n){
                err_sys("write error to pipe");
            }
            //读管道
            if((n=read(fd[0],line,MAXLINE))<0){
                err_sys("read error from pipe");
            }
            //管道关闭
            if(0==n){
                err_msg("child close pipe");
                break;
            }
            //终端输出
            line[n]=0;
            if(EOF==fputs(line,stdout)){
                err_sys("fputs error");
            }
        }//标准输入中读取(while)
        //如果标准输入出错
        if(ferror(stdin)){
            err_sys("fegts error on stdin");
        }
        //退出
        exit(0);
    }//父进程
    //子进程
    else{
        //关闭写管道
        close(fd[0]);
        //设置读管道为标准输入
        if((fd[1]!=STDIN_FILENO)&&(dup2(
            fd[1],STDIN_FILENO)!=STDIN_FILENO)){
            err_sys("dup2 error to stdin");
        }
        //设置读管道为标准输出
        if((fd[1]!=STDOUT_FILENO)&&(dup2(
            fd[1],STDOUT_FILENO)!=STDOUT_FILENO)){
            err_sys("dup2 error to stdout");
        }
        //执行程序
        if(execl("./add2","add2",(char *)0)<0){
            err_sys("execl error");
        }
    }//子进程
    //退出进程
    exit(0);
}

//信号捕获函数
static void sig_pipe(int /*signo*/){
    printf("SIGPIPE caught\n");
    exit(1);
}
