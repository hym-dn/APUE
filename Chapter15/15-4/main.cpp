#include"../../apue.h"
#include<sys/wait.h>

//分页命令
#define PAGER "${PAGER:-more}" /* environment variable, or default */

int main(int argc,char *argv[]){
    //行字符
    char line[MAXLINE];
    //IO文件描述符
    FILE *fpin,*fpout;
    //参数非法
    if(argc!=2){
        err_quit("usage: a.out<pathname>");
    }
    //打开文件
    if((fpin=fopen(argv[1],"r"))==NULL){
        err_sys("can't open %s",argv[1]);
    }
    //分页
    if((fpout=popen(PAGER,"w"))==NULL){//popen返回命令的标准输入
        err_sys("popen error");
    }
    //拷贝文件
    if(fgets(line,MAXLINE,fpin)!=NULL){
        if(fputs(line,fpout)==EOF){
            err_sys("fputs error to pipe");
        }
    }
    //文件异常
    if(ferror(fpin)){
        err_sys("fgets error");
    }
    //关闭文件
    if(pclose(fpout)==-1){//pclose关闭文件描述符，等待命令执行结束，返回shell的终止状态
        err_sys("pclose error");
    }
    //退出程序
    exit(0);
}
