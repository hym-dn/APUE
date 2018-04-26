#include"../../apue.h"

int main(int argc,char *argv[]){
    // 如果参数不为3
    if(argc!=3){
        err_quit("usage: a.out <directory> <prefix>");
    }
    // 打印临时文件名称
    printf("%s\n",tempnam(argv[1][0]!=' '?argv[1]:NULL,
        argv[2][0]!=' '?argv[2]:NULL));
    // 退出
    exit(0);
}