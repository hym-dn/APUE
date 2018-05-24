#include"../../apue.h"
#include<sys/wait.h>

int main(void){
    int status;
    // 执行"date"命令
    if((status=system("date"))<0){ // 出错
        err_sys("system() error");
    }
    // 打印执行结果
    pr_exit(status);
    // 执行"nosuchcommand"命令
    if((status=system("nosuchcommand"))<0){ // 出错
        err_sys("system() error");
    }
    // 打印执行结果
    pr_exit(status);
    // 执行“who;exit 44”命令
    if((status=system("who;exit 44"))<0){
        err_sys("system() error");
    }
    // 打印执行结果
    pr_exit(status);
    // 退出
    exit(0);
}