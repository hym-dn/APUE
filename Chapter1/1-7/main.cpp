#include"../../apue.h"

int main(void){
    //打印用户ID、组ID
    printf("uid=%d,gid=%d\n",getuid(),getgid());
    //退出
    exit(0);
}

