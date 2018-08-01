#include"../../apue.h"
#include<fcntl.h>
#include<stropts.h>
#include<sys/conf.h>

int main(int argc,char *argv[]){
    int fd,i,nmods;
    // 如果参数个数非法
    if(argc!=2){
        err_quit("usage: %s <pathname>",argv[0]);
    }
    // 打开文件
    if((fd=open(argv[1],O_RDONLY))<0){
        err_sys("can't open %s",argv[1]);
    }
    // 不是流设备
    if(isastream(fd)==0){
        err_quit("%s is not a stream",argv[i]);
    }
    // 获取模块的数量
    /**
     * Fetch number of modules.
     */
    if((nmods=ioctl(fd,I_LIST,(void*)0))<0){
        err_sys("I_LIST error for nmods");
    }
    printf("#modules=%d\n",nmods);
    // 为模块名字分配存储空间
    /**
     * Allocate storage for all the module names. 
     */
    list.sl_modlist=calloc(nmods,sizeof(struct str_mlist));
    if(list.sl_modlist==NULL){
        err_sys("calloc error");
    }
    list.sl_nmods=nmods;
    // 获取模块名称
    /**
     * Fetch the module names. 
     */
    if(ioctl(fd,I_LIST,&list)<0){
        err_sys("I_LIST error for list");
    }
    // 打印名称
    /**
     * Print the names. 
     */
    for(i=1;i<=nmods;i++){
        printf(" %s: %s\n",(i==nmods)?"driver":
            "module",list.sl_modlist++->l_name);
    }
    // 退出
    exit(0);
}