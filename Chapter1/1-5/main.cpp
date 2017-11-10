#include"../../apue.h"
#include<sys/wait.h>

int main(void){
    //行缓冲区
    char buf[MAXLINE];
    //进程ID
    pid_t pid;
    //子进程退出状态
    int status;
    //打印提示
    printf("%% ");
    //读取一行
    while(fgets(buf,MAXLINE,stdin)!=NULL){
        //字符串结束符替换
        if(buf[strlen(buf)-1]=='\n'){
            buf[strlen(buf)-1]=0;
        }
        //创建子进程
        if((pid=fork())<0){
            err_sys("fork error");
        }
        //子进程
        else if(pid==0){
            //执行命令
            execlp(buf,buf,(char *)0);
            //出错
            err_ret("couldn't execute: %s",buf);
            //退出
            exit(127);
        }
        //父进程
        if((pid=waitpid(pid,&status,0))<0){
            err_sys("waitpid error");
        }
        //打印提示
        printf("%%");
    }
    //退出
    exit(0);
}
