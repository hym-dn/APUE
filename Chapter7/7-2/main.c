#include"../../apue.h"

static void my_exit1(void);
static void my_exit2(void);

int main(void){
    // 安装第一个退出函数
    if(atexit(my_exit2)!=0){ // 失败
        err_sys("can't register my_exit2");
    }
    // 安装第二个退出函数
    if(atexit(my_exit1)!=0){
        err_sys("can't register my_exit1");
    }
    // 安装第三个退出函数
    if(atexit(my_exit1)!=0){
        err_sys("can't register my_exit1");
    }
    // 输出信息
    printf("main is done\n");
    // 退出
    return(0);
}

// 退出函数
static void my_exit1(void){
    printf("first exit handler\n");
}

// 退出函数
static void my_exit2(void){
    printf("second exit handler\n");
}