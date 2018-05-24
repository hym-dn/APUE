#include"../../apue.h"

int main(int argc,char *argv[]){
    // 定义返回
    int status;
    // 参数小于两个
    if(argc<2){
        err_quit("command-line argument required");
    }
    // 利用system执行命令
    if((status=system(argv[1]))<0){
        err_sys("system() error"); // 执行失败
    }
    // 打印执行结果
    pr_exit(status);
    // 退出
    exit(0);
}