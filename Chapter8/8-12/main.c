#include<sys/wait.h>
#include<errno.h>
#include<unistd.h>

// 没有进行信号处理的system函数实现
int system(const char *cmdstring){ /* version without signal handling */
    // 进程ID
    pid_t pid;
    // 记录返回值
    int status;
    // 如果命令为空,则返回非0,说明系统支持此函数调用
    if(cmdstring==NULL){
        return(1); /* always a command processor with UNIX */
    }
    // 创建子进程
    if((pid=fork())<0){ // 失败
        status=-1; /* probably out of processes */
    }
    // 子进程
    else if(0==pid){
        // 执行命令
        execl("/bin/sh","sh","-c",cmdstring,(char*)0);
        // 退出
        _exit(127); /* execl error */ // 如果execl执行失败，则其返回值如同shell执行了exit(127)一样。
    }
    // 父进程
    else{ /* parent */
        // 等待子进程结束
        while(waitpid(pid,&status,0)<0){ // 失败
            // 非中断错误
            if(errno!=EINTR){
                status=-1; /* error other than EINTR from waitpid() */
                break;
            }
            // 中断错误不返回
        }
    }
    // 返回
    return(status);
}