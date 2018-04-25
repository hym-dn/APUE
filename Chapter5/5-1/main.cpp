#include"../../apue.h"

int main(void){
    int c;
    // 从标准输入读
    while((c=getc(stdin))!=EOF){
        // 放到标准输出
        if(putc(c,stdout)==EOF){
            err_sys("output error");
        }
    }
    // 如果标准输入出错
    if(ferror(stdin)){
        err_sys("input error");
    }
    exit(0);
}