#include"../../apue.h"

int main(int argc,char *argv[]){
    int i;
    // 打印全部的命令行参数
    for(i=0;i<argc;i++) /* echo all command-line args */
        printf("argv[%d]: %s\n",i,argv[i]);
    // 退出
    exit(0);
}