#include"../../apue.h"

int main(void){
    // 记录进程ID
    pid_t pid;
    // 创建子进程
    if((pid=fork())<0){ // 失败
        err_sys("fork error");
    }
    // 父进程
    else if(pid!=0){ /* parent */
        sleep(2);
        exit(2); /* terminate with exit status 2 */
    }
    // 第一个子进程 /* first child */
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 父进程
    else if(pid!=0){
        sleep(4);
        abort(); /* terminate with core dump */       
    }
    // 第二个子进程
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 父进程
    else if(pid!=0){
        // 执行命令
        execl("/bin/dd","dd","if=/etc/termcap","of=/dev/null",NULL);
        // 不会被执行的内容
        exit(7); /* shouldn't get here */
    }
    // 第三个子进程
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    // 父进程
    else if(pid!=0){
        sleep(8);
        exit(0); /* normal exit */
    }
    // 第四个子进程 /* fourth child */
    sleep(6);
    kill(getpid(),SIGKILL); /* terminate w/signal,no core dump */
    exit(6);
}