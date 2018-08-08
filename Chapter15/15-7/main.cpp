#include"../../apue.h"
#include<sys/wait.h>

int main(void){
    //行内容
    char line[MAXLINE];
    //文件流
    FILE *fpin;
    //1.Fork子进程
    //2.执行15-6程序
    //3.返回15-6的标准输出
    if((fpin=popen("./myuclc","r"))==0){
        err_sys("popen error");
    }
    //显示15-6过滤程序的过滤结果
    for(;;){
        //向标准输出输出字符
        fputs("prompt> ",stdout);
        //冲洗标准输出
        fflush(stdout);
        //从管道中读取内容
        if(fgets(line,MAXLINE,fpin)==0){
            break;
        }
        //输出读取内容
        if(fputs(line,stdout)==EOF){
            err_sys("fputs error to pipe");
        }
    }
    //关闭文件流，等待指令执行完成
    if(pclose(fpin)==-1){
        err_sys("pclose error");
    }
    //输出回车
    putchar('\n');
    //退出
    exit(0);
}