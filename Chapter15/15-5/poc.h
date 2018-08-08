#include"../../apue.h"
#include<errno.h>
#include<fcntl.h>
#include<sys/wait.h>

//子进程、文件描述符匹配数组
/**
 * Pointer to array allocated at run-time.
 */
static pid_t *childpid=NULL;

//打开文件描述符限制
/**
 * From our open_max(),Figure 2.16. 
 */
static int maxfd;

// 自己实现popen函数
// 1.fork子进程
// 2.调用exec执行命令
// 3.返回标准输入或标准输出
FILE *popen(const char *cmdstring,const char *type){
    // 循环变量
    int i;
    // 管道
    int pfd[2];
    // 进程ID
    pid_t pid;
    // 文件描述符
    FILE *fp;
    // 只允许输入'r'或'w'
    /* only allow "r" or "w" */
    if((type[0]!='r'&&type[0]!='w')||type[1]!=0){
        // 错误编号
        errno=EINVAL; /* required by POSIX */
        // 返回空
        return(NULL);
    }
    // 子进程文件描述匹配数组尚未创建
    if(childpid==NULL){ /* first time through */
        // 进程文件描述符数量限制
        /* allocate zeroed out array for child through */
        maxfd=open_max();
        // 创建子进程文件描述符匹配数组
        if((childpid=(pid_t*)calloc(
            maxfd,sizeof(pid_t)))==NULL){
            return(NULL);
        }
    }
    //创建管道
    if(pipe(pfd)<0){ // 失败
        return(NULL); /* errno set by pipe() */
    }
    //创建子进程
    if((pid=fork())<0){ // 失败
        return(NULL); /* errno set by fork() */
    }
    //子进程
    else if(pid==0){ /* child */
        //'读'标识
        if(*type=='r'){
            //关闭'读'管道
            close(pfd[0]);
            //如果标准输出不是写管道
            if(pfd[1]!=STDOUT_FILENO){
                //设置标准输出为写管道
                dup2(pfd[1],STDOUT_FILENO);
                //关闭原写管道
                close(pfd[1]);
            }
        }
        //‘写’标识
        else{
            //关闭‘写’管道
            close(pfd[1]);
            //如果标准输入非读管道
            if(pfd[0]!=STDIN_FILENO){
                //将读管道设置为标准输入
                dup2(pfd[0],STDIN_FILENO);
                //关闭原读管道
                close(pfd[0]);
            }
        }
        //关闭全部的文件描述符
        /* close all descriptors in childpid[] */
        for(i=0;i<maxfd;i++){
            if(childpid[i]>0){
                close(i);
            }
        }
        //执行指令
        execl("/bin/sh","sh","-c",cmdstring,(char*)0);
        //退出
        _exit(127);
    }
    //父进程
    /* parent continues ... */
    //'读'标识
    if(*type=='r'){
        //关闭写管道
        close(pfd[1]);
        //取得读管道的文件流
        if((fp=fdopen(pfd[0],type))==NULL){
            return(NULL);
        }
    }
    //'写'标识
    else{
        //关闭读管道
        close(pfd[0]);
        //取得写管到的文件流
        if((fp=fdopen(pfd[1],type))==NULL){
            return(NULL);
        }
    }
    //记录打开文件流的子进程
    childpid[fileno(fp)]==pid; /* remember child pid for this fd */
    //返回文件流
    return(fp);
}

int pclose(FILE *fp){
    //文件描述符、文件状态
    int fd,stat;
    //进程ID
    pid_t pid;
    //如果popen尚未调用
    if(childpid==NULL){
        //设置错误
        errno=EINVAL;
        //返回错误
        return(-1); /* popen() has never been called */
    }
    //取得输入文件流文件描述符
    fd=fileno(fp);
    //获取输入流的进程ID
    if((pid=childpid[fd])==0){
        //设置错误
        errno=EINVAL;
        //返回错误
        return(-1); /* fp wasn't opened by open() */
    }
    //清空相应子进程
    childpid[fd]=0;
    //关闭相应文件流
    if(fclose(fp)==EOF){
        return(-1);
    }
    //等待进程退出
    while(waitpid(pid,&stat,0)<0){
        if(errno!=EINTR){
            return(-1); /* error other than EINTR from waitpid() */
        }
    }
    //返回进程退出状态
    return(stat); /* return child's termination status */
}