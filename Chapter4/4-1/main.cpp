#include"../../apue.h"

int main(int argc,char *argv[]){
    int i;
    struct stat buf;
    char *ptr;
    // 遍历输入参数
    for(i=1;i<argc;i++){
        // 打印输入参数
        printf("%s: ",argv[i]);
        // 获取指定文件的信息结构，如果是符号连接，
        // 则返回符号连接文件本身的信息结构
        if(lstat(argv[i],&buf)<0){ // 失败
            // 显示错误信息
            err_ret("lstat error");
            // 继续下次循环
            continue;            
        }
        // 普通文件
        if(S_ISREG(buf.st_mode)){
            ptr="regular";
        }
        // 目录文件
        else if(S_ISDIR(buf.st_mode)){
            ptr="directory";
        }
        // 字符文件
        else if(S_ISCHR(buf.st_mode)){
            ptr="character special";
        }
        // 块文件
        else if(S_ISBLK(buf.st_mode)){
            ptr="block special";
        }
        // FIFO文件
        else if(S_ISFIFO(buf.st_mode)){
            ptr="fifo";
        }
        // 符号链接文件
        else if(S_ISLNK(buf.st_mode)){
            ptr="symbolic link";
        }
        // 套接子文件
        else if(S_ISSOCK(buf.st_mode)){
            ptr="socket";
        }
        // 未知
        else{
            ptr="** unknown mode **";
        }
        // 打印
        printf("%s\n",ptr);
    }
    // 退出
    exit(0);
}