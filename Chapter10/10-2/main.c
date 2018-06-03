#include"../../apue.h"
#include<pwd.h>

// 报警信号捕捉函数
static void my_alarm(int signo){
    // 用户信息
    struct passwd *rootptr;
    // 输出提示信息
    printf("in signal handler\n");
    // 获取指定用户登陆信息
    if((rootptr=getpwnam("root"))==NULL){
        err_sys("getpwnam(root) error");
    }
    // 安装定时器
    alarm(1);
}

int main(void){
    // 用户登录信息
    struct passwd *ptr;
    // 安装信号捕捉
    signal(SIGALRM,my_alarm);
    // 安装定时器
    alarm(1);
    // 无限循环
    for(;;){
        // 获取指定用户登录信息
        if((ptr=getpwnam("heyameng"))==NULL){ // 失败
            err_sys("getpwnam error");
        }
        // 打印用户登录信息
        if(strcmp(ptr->pw_name,"heyameng")!=0){
            printf("return value corrupted!,pw_name=%s\n",
                ptr->pw_name);
        }
    }
}