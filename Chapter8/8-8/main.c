#include"../../apue.h"
#include<sys/wait.h>

char *env_init[]={"USER=unknown","PATH=/tmp",NULL};

int main(void){
    pid_t pid;
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error"); // 输出错误
    }
    // 子进程
    else if(pid==0){ /* specify pathname,specify environment */
        if(execle("/home/sar/bin/echoall","echoall","myarg1",
            "MY ARG2",(char *)0,env_init)<0){ // 如果execle执行成功，
            err_sys("execle error");          // 这段代码将不会被执行
        }
    }
    // 等待子进程结束
    if(waitpid(pid,NULL,0)<0){
        err_sys("wait error");
    }
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error"); // 输出错误
    }
    // 子进程
    else if(pid==0){ /* specify pathname,specify environment */
        if(execlp("echoall","echoall","only 1 arg",
            (char *)0)<0){ // 如果execle执行成功，
            err_sys("execlp error");          // 这段代码将不会被执行
        }
    }
    // 退出
    exit(0);
}