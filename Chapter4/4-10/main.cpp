#include"../../apue.h"

#ifdef SOLARIS
#include<sys/mkdev.h>
#endif

int main(int argc,char *argv[]){
    // 变量
    int i;
    struct stat buf;
    // 遍历参数
    for(i=1;i<argc;i++){
        // 打印
        printf("%s: ",argv[i]);
        // 获取指定文件信息
        if(stat(argv[i],&buf)<0){
            // 输出错误
            err_ret("stat error");
            // 继续
            continue;
        }
        // 打印主、次设备号
        printf("dev=%d/%d",major(buf.st_dev),minor(buf.st_dev));
        // 字符设备、块设备
        if(S_ISCHR(buf.st_mode)||S_ISBLK(buf.st_mode)){
            printf(" (%s) rdev=%d/%d",(S_ISCHR(buf.st_mode))?
                "character":"block",major(buf.st_rdev),minor
                (buf.st_rdev)); // 输出
        }
        // 回车
        printf("\n");
    }
    // 退出
    exit(0);
}