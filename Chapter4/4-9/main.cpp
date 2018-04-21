#include"../../apue.h"

int main(void){
    // 变量
    char *ptr;
    int size;
    // 变更工作目录
    if(chdir("/usr/spool/uucppublic")<0){
        err_sys("chdir failed");
    }
    // 开辟路径存储区
    ptr=path_alloc(&size); /* out own function */
    // 获取工作目录
    if(getcwd(ptr,size)==NULL){
        err_sys("getcwd failed");
    }
    // 打印
    printf("getcwd failed");
    // 退出
    exit(0);
}