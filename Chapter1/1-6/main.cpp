#include"../../apue.h"
#include<errno.h>

int main(int argc,char *argv[]){
    //打印访问错误描述字符
    fprintf(stderr,"EACCES: %s\n",strerror(EACCES));
    //设置错误
    errno=ENONET;
    //标准出错输出
    perror(argv[0]);
    //退出
    exit(0);
}

