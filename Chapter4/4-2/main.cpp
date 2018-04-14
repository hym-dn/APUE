#include"../../apue.h"
#include<fcntl.h>

int main(int argc,char *argv[]){
    // 如果输入参数个数不合法
    if(argc!=2){
        err_quit("usage: a.out <pathname>");
    }
    // 读权限探测
    if(access(argv[1],R_OK)<0){
        err_ret("access error for %s",argv[1]);
    }else{
        printf("read access OK\n");
    }
    // 只读打开
    if(open(argv[1],O_RDONLY)<0){
        err_ret("open error for %s",argv[1]);
    }else{
        printf("open for reading OK\n");
    }
    // 退出
    exit(0);
}