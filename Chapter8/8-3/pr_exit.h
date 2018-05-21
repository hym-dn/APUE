#include"../../apue.h"
#include<sys/wait.h>

void pr_exit(int status){
    // 若为正常终止终止子进程返回的状态，则为真。
    // 对于这种情况可执行WEXITSTATUS（status），
    // 取子进程传动给exit、_exit或_Exit参数的
    // 低8位
    if(WIFEXITED(status)){
        printf("normal termination,exit status=%d\n",
            WEXITSTATUS(status));
    }
    // 若为异常终止子进程返回的状态，则为真（接到
    // 一个不捕捉的信号）。对于这种情况，可执行
    // WTERMSGI（status），取使子进程终止的信号
    // 编号。另外，有些实现（非Single UNIX Specification）
    // 定义宏WCOREDUMP（status），若已产生终止进程
    // 的core文件，则它返回真
    else if(WIFSIGNALED(status)){
        printf("abnormal termination,signal number=%d%s\n",
            WTERMSIG(status),
#ifdef WCOREDUMP
            WCOREDUMP(status)?" (core file generated)":"");
#else
            "");
#endif
    }
    // 若为当前暂停子进程的返回的状态，则为真。对于这种情况，可
    // 执行WSTOPSIG(status)，取得使子进程暂停的信号编号
    else if(WIFSTOPPED(status)){
        printf("child stopped,signal number=%d\n",
            WSTOPSIG(status));
    }
}