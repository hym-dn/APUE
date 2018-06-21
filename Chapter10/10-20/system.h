#include<sys/wait.h>
#include<errno.h>
#include<signal.h>
#include<unistd.h>

// 处理信号的system版本
int system(const char *cmdstring){ /* with appropriate signal handling */
    pid_t pid;
    int status;
    struct sigaction ignore,saveintr,savequit;
    sigset_t chldmask,savemask;
    // 未输入指令
    if(cmdstring==NULL){
        return(1); /* always a command processor with UNIX */
    }
    // 忽略SIGINT和SIGQUIT信号
    ignore.sa_handler=SIG_IGN; /* ignore SIGINT and SIGQUIT */
    sigemptyset(&ignore.sa_mask); // 清空屏蔽字
    if(sigaction(SIGINT,&ignore,&saveintr)<0){ // 忽略SIGINT
        return(-1);
    }
    if(sigaction(SIGQUIT,&ignore,&savequit)<0){ // 忽略SIGQUIT
        return(-1);
    }
    // 阻塞SIGCHLD
    sigemptyset(&chldmask); /* now block SIGCHLD */
    sigaddset(&chldmask,SIGCHLD); // 增加屏蔽字
    if(sigprocmask(SIG_BLOCK,&chldmask,&savemask)<0){
        return(-1);
    }
    // 创建子进程
    if((pid=fork())<0){ // 创建子进程
        status=-1; /* probably out of processes */
    }
    // 子进程
    else if(pid==0){ /* 子进程 */
        /* restore previous signal actions & reset signal mask */
        // 恢复先前信号动作，并且重置信号屏蔽字
        sigaction(SIGINT,&saveintr,NULL);
        sigaction(SIGQUIT,&savequit,NULL);
        sigprocmask(SIG_SETMASK,&savemask,NULL);
        // 执行命令
        execl("/bin/sh","sh","-c",cmdstring,(char*)0);
        // 错误退出
        _exit(127); /* exec error */
    }
    // 父进程
    else{ /* parent */
        // 等待子进程退出
        while(waitpid(pid,&status,0)<0){ // 失败
            if(errno!=EINTR){ // 不是被中断
                status=-1; /* error other than EINTR from waitpid() */
                break;
            }
        }
    }
    /* restore previous signal actions & reset signal mask */
    // 恢复先前信号动作，并且重置信号屏蔽字
    if(sigaction(SIGINT,&saveintr,NULL)<0){
        return(-1);
    }
    if(sigaction(SIGQUIT,&savequit,NULL)<0){
        return(-1);
    }
    if(sigprocmask(SIG_SETMASK,&savemask,NULL)<0){
        return(-1);
    }
    // 返回
    return(status);
}