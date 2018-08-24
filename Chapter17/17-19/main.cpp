#include"../17-18/open.h"
#include<fcntl.h>

#define BUFFSIZE 8192

int main(int argc,char *argv[]){
    int n,fd;
    char buf[BUFFSIZE],line[MAXLINE];
    // 循环从标准输入中读取一行
    /* read filename to cat from stdin */
    while(fgets(line,MAXLINE,stdin)!=NULL){
        // 增加字符串结束符
        if(line[strlen(line)-1]=='\n'){
            line[strlen(line)-1]=0; /* replace newline with null */
        }
        // 与服务器取得联系，打开指定的文件描述符
        /* open the file */
        if((fd=csopen(line,O_RDONLY))<0){
            continue; /* csopen() prints error from server */
        }
        // 读取文件内容，并输出到标准输出
        /* and cat to stdout */
        while((n=read(fd,buf,BUFFSIZE))>0){
            if(write(STDOUT_FILENO,buf,n)!=n){
                err_sys("write error");
            }
        }
        // 如果读取失败
        if(n<0){
            err_sys("read error");
        }
        // 关闭文件描述符
        close(fd);
    }
    // 退出
    exit(0);
}
