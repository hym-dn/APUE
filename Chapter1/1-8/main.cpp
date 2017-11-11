#include"../../apue.h"
#include<sys/wait.h>

//中断捕获
static void sig_int(int);

int main(void){
    //读缓冲区
    char buf[MAXLINE];
    //进程ID
    pid_t pid;
    //进程退出状态
    int status;
    //捕获信号
    if(signal(SIGINT,sig_int)==SIG_ERR){
        err_sys("signal error");
    }
    //打印提示
    printf("%% ");
    //循环读
    while(fgets(buf,MAXLINE,stdin)!=NULL){
        //一终止符替换换行符
        if(buf[strlen(buf)-1]=='\n'){
            buf[strlen(buf)-1]=0;
        }
        //创建进程
        if((pid=fork())<0){
            err_sys("fork error");
        }
        //子进程
        else if(0==pid){
            //执行输入
            execlp(buf,buf,(char*)0);
            //输出错误
            err_ret("couldn't execute: %s",buf);
            //退出
            exit(127);
        }
        //父进程
        //等待子进程退出
        if((pid=waitpid(pid,&status,0))<0){
            err_sys("waitpid error");
        }
        //打印提示
        printf("%% ");
    }
    //退出
    exit(0);
}

//中断捕获函数
void sig_init(int /*signo*/){
    printf("interrupt\n%%");
}
