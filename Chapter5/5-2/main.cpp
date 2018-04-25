#include"../../apue.h"

int main(void){
    char buf[MAXLINE];
    // 从标准输入读入
    while(fgets(buf,MAXLINE,stdin)!=NULL){
        // 输出到标准输出
        if(fputs(buf,stdout)==EOF){
            err_sys("output error");
        }
    }
    // 如果出错
    if(ferror(stdin)){
        err_sys("input error");
    }
    // 退出
    exit(0);
}