#include"../../apue.h"
#include<dirent.h>

int main(int argc,char *argv[]){
    //目录指针
    DIR *dp;
    //目录信息结构体
    struct dirent *dirp;
    //输入参数非法
    if(argc!=2){
        err_quit("usage: ls directory_name");
    }
    //打开指定目录
    if((dp=opendir(argv[1]))==NULL){
        err_sys("can't open %s",argv[1]);
    }
    //遍历打印全部目录
    while((dirp=readdir(dp))!=NULL){
        printf("%s\n",dirp->d_name);
    }
    //关闭目录
    closedir(dp);
    //退出程序
    exit(0);
}

