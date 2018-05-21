#include"../../apue.h"

static void charatatime(char *);

int main(void){
    // 进程ID
    pid_t pid;
    // 创建子进程
    if((pid=fork())<0){
        err_sys("fork error"); // 输出错误
    }
    // 子进程
    else if(pid==0){
        charatatime("output from child\n");
    }
    // 父进程
    else{
        charatatime("output from parent\n");
    }
    // 退出
    exit(0);
}

static void charatatime(char *str){
    // 声明变量
    char *ptr;
    int c;
    // 设置标准输出为非缓冲
    setbuf(stdout,NULL); /* set unbuffered */
    // 向标准输出输出字符串
    for(ptr=str;(c=*ptr++)!=0;){
        putc(c,stdout);
    }
}