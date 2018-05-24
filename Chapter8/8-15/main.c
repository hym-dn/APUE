#include"../apue.h"

int main(void){
    // 打印实际用户ID、有效用户ID
    printf("real uid=%d,effective uid=%d\n",getuid(),geteuid());
    // 推出
    exit(0);
}