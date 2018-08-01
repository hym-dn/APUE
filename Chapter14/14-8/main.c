#include"../../apue.h"
#include"../14-7/isastream.h"
#include<fcntl.h>

int main(int argc,char *argv[]){
    // 声明变量
    int i,fd;
    // 遍历参数
    for(i=1;i<argc;i++){
        // 打开指定文件
        if((fd=open(argv[i],O_RDONLY))<0){
            err_ret("%s: can't open",argv[i]);
            continue;
        }
        // 查看是否为流设备
        if(isastream(fd)==0){
            err_ret("%s: not a stream",argv[i]);
        }else{
            err_msg("%s: streams device",argv[i]);
        }
    }
    // 退出
    exit(0);
}