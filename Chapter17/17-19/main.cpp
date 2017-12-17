#include"open.h"
#include<fcntl.h>

#define BUFFSIZE 8192

int main(int argc,char *argv[]){
    int n,fd;
    char buf[BUFFSIZE];
    char line[MAXLINE];
    //从标准输入中读取一行
    while(fgets(line,MAXLINE,stdin)!=NULL){
        //重写结束符
        if(line[strlen(line)-1]=='\n'){
            line[strlen(line)-1]=0;
        }
        //与服务器取得联系，打开指定的文件描述符
        if((fd=csopen(line,O_RDONLY))<0){
            continue;
        }
        //读取文件内容，并输出的标准输出
        while((n=read(fd,buf,BUFFSIZE))>0){
            if(write(STDOUT_FILENO,buf,n)!=n){
                err_sys("write error");
            }
        }
        //如果读取失败
        if(n<0){
            err_sys("read error");
        }
        //关闭文件描述符
        close(fd);
    }
    //退出
    exit(0);
}
