#include"../../apue.h"
#include<sys/wait.h>

//分页程序
#define DEF_PAGER "/bin/more"

int main(int argc,char *argv[]){
    //读取长度
    int n;
    //管道描述符
    int fd[2];
    //进程ID
    pid_t pid;
    //分页程序
    char *pager,*argv0;
    //行字符
    char line[MAXLINE];
    //文件指针
    FILE *fp;
    //参数输入非法
    if(argc!=2){
        err_quit("usage: a.out <pathname>");
    }
    //分页文件
    if((fp=fopen(argv[1],"r"))==NULL){
        err_sys("can't open %s",argv[1]);
    }
    //创建管道
    if(pipe(fd)<0){
        err_sys("pipe error");
    }
    //子进程
    if((pid=fork())<0){
        err_sys("fork error");
    }
    //父进程
    else if(pid>0){
        //关闭'读'
        close(fd[0]);
        //读文件
        while(fgets(line,MAXLINE,fp)!=NULL){
            //读取长度
            n=strlen(line);
            //写管道
            if(write(fd[1],line,n)==NULL){
                err_sys("write error to pipe");
            }
        }
        //检测出错
        if(ferror(fp)){
            err_sys("fgets error");
        }
        //关闭'写'
        close(fd[1]);
        //等待子进程结束
        if(waitpid(pid,NULL,0)<0){
            err_sys("fgets error");
        }
        //退出
        exit(0);
    }
    //子进程
    else{
        //关闭'写'
        close(fd[1]);
        //复制'读'
        if(fd[0]!=STDIN_FILENO){
            //复制'读'到标准输入
            if(dup2(fd[0],STDIN_FILENO)!=STDIN_FILENO){
                err_sys("dup2 error to stdin");
            }
            //关闭'读'
            close(fd[0]);
        }
        //分页程序
        if((pager=getenv("PAGER"))==NULL){
            pager=DEF_PAGER;
        }
        //分页程序名称
        if((argv0=strrchr(pager,'/'))!=NULL){
            argv0++;
        }else{
            argv0=pager;
        }
        //执行分页
        if(execl(pager,argv0,(char*)0)<0){
            err_sys("execl error for %s",pager);
        }
    }
    //退出
    exit(0);
}
