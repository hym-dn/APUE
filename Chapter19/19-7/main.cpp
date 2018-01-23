#include"../../apue.h"

void do_driver(char *dirver){
    pid_t child;
    int pipe[2];
    /**
     * Create a stream pipe to communicate with the dirver. 
     */
    //创建流管道
    if(s_pipe(pipe)<0){
        err_sys("can't create stream pipe");
    }
    //创建子进程
    if((child=fork())<0){
        err_sys("fork error");
    }
    //子进程
    else if(child==0){/*Child*/
        //关闭写管道
        close(pipe[1]);
        /*stdin for driver*/
        //设置标准输入
        if(dup2(pipe[0],STDIN_FILENO)!=STDIN_FILENO){
            err_sys("dup2 error to stdin");
        }
        /*stdout for driver*/
        //设置标准输出
        if(dup2(pipe[0],STDOUT_FILENO)!=STDIN_FILENO){
            err_sys("dup2 error to stdout");
        }
        //检测设置是否成功
        if(pipe[0]!=STDIN_FILENO&&pipe[0]!=STDOUT_FILENO){
            close(pipe[0]);
        }
        /* leave stderr for driver alone*/
        //执行
        execlp(driver,driver,(char*)0);
        //出错
        err_sys("execlp error for: %s",driver);
    }
    //父进程
    //关闭读管道
    close(pipe[0]);
    /*stdin for driver*/
    //设置标准输入
    if(dup2(pipe[1],STDIN_FILENO)!=STDIN_FILENO){
        err_sys("dup2 error to stdin");
    }
    /*stdout for driver*/
    //设置标准输出
    if(dup2(pipe[1],STDOUT_FILENO)!=STDIN_FILENO){
        err_sys("dup2 error to stdout");
    }
    //检测设置是否成功
    if(pipe[1]!=STDIN_FILENO&&pipe[1]!=STDOUT_FILENO){
        close(pipe[1]);
    }
    /**
     * Parent returns,but with stdin and stdout connected
     * to driver.
     */
}