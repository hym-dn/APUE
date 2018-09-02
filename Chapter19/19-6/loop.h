#include"../../apue.h"

#define BUFFSIZE 512

static void sig_term(int);
static volatile sig_atomic_t sigcaught; /* set by signal handler */

/**
 * 将标准输入接收到的所有内容复制到伪终端主设备
 * 将伪终端主设备接收到的所有内容复制到标准输出
 */
void loop(int ptym,int ignoreeof){
    pid_t child;
    int nread;
    char buf[BUFFSIZE];
    // 创建子进程
    if((child=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 子进程
    else if(child==0){ /* child copies stdin to ptym */
        // 循环将标准输入内容写到伪终端中
        for(;;){
            // 从标准输入读
            if((nread=read(STDIN_FILENO,buf,BUFFSIZE))<0){
                err_sys("read error from stdin");
            }
            // 读到结尾
            else if(0==nread){
                break; /* EOF on stdin means we're done */
            }
            // 写标准输入到伪终端
            if(writen(ptym,buf,nread)!=nread){
                err_sys("writen error to master pty");
            }
        }
        /**
         *  We always terminate when we encounter an EOF on stdin,
         *  but we notify the parent only if ignoreeof is 0.
         */
        // 如果不忽略结束符
        if(ignoreeof==0){
            // 通知父进程
            kill(getppid(),SIGTERM); /* notify parent */
        }
        // 退出
        exit(0); /* and terminate;child can't return*/
    }
    // 父进程
    /**
     * Parent copies ptym to stdout. 
     */
    // 捕获信号
    if(signal_intr(SIGTERM,sig_term)==SIG_ERR){
        err_sys("signal_intr error for SIGTERM");
    }
    // 循环读取伪终端内容，并且写到标准输出中
    for(;;){
        // 读取伪终端
        if((nread=read(ptym,buf,BUFFSIZE))<=0){
            break; /* signal caught,error,or EOF */
        }
        // 写标准输出
        if(writen(STDOUT_FILENO,buf,nread)!=nread){
            err_sys("writen error to stdout");
        }
    }
    /**
     * There are three ways to get here: sig_term() below caught the
     * SIGTERM from the child,we read an EOF on the pty master (which
     * means we have to signal the child to stop),or an error.
     */
    // 如果信号没有被捕获
    if(sigcaught==0){ /* tell child if it didn't send us the signal */
        // 通知子进程
        kill(child,SIGTERM);
    }
    /**
     * Parent returns to caller.
     */
}

/**
 * The child sends us SIGTERM when it gets EOF on the pty slave or
 * when read() fails. We probably interrupted the read() of ptym. 
 */
// 中断读
static void sig_term(int signo){
    sigcaught=1; /* just set flag and return*/
}