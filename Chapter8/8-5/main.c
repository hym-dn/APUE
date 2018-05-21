#include"../../apue.h"
#include<sys/wait.h>

int main(void){
    // 存储进程ID
    pid_t pid;
    // 创建子进程
    if((pid=fork())<0){ // 创建失败
        err_sys("fork error"); 
    }
    // 子进程
    else if(pid==0){ /* first child */
        // 创建子进程
        if((pid=fork())<0){
            err_sys("fork error");
        }
        // 父亲进程（第一个子进程）
        else if(pid>0){
            exit(0); /* parent from second fork == first child */
        }
        // 第二个子进程
        /**
         * We're the second child; our parent becomes init as soon
         * as our real parent calls exit() in the statement above.
         * Here's where we'd continue executing,knowing that when
         * we're done, init will reap our status. 
         */
        sleep(2); // 等待父进程结束，自己变为孤儿进程
        // 打印信息
        printf("second child,parent pid=%d\n",getppid());
        // 退出
        exit(0);
    }
    // 父进程
    // 等待第一个子进程结束
    if(waitpid(pid,NULL,0)!=pid){/* wait for first child */
        err_sys("waitpid error");
    }
    /**
     * We're the parent (the original process); we continue executing,
     * knowing that we're not the parent of the second child.
     */
    exit(0);
}