#include"../../apue.h"
#include<fcntl.h>
#include<sys/mman.h>

int main(int argc,char *argv[]){
    int fdin,fdout;
    void *src,*dst;
    struct stat statbuf;
    // 如果参数数量非法
    if(argc!=3){
        err_quit("usage: %s <fromfile> <tofile>",argv[0]);
    }
    // 打开源文件
    if((fdin=open(argv[1],O_RDONLY))<0){ // 打开失败
        err_sys("can't open %s for reading",argv[1]);
    }
    // 打开目标文件
    if((fdout=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,FILE_MODE))<0){
        err_sys("can't open %s for reading",argv[1]);
    }
    // 获取源文件属性
    if(fstat(fdin,&statbuf)<0){ /* need size of input file*/
        err_sys("fstat error");
    }
    // 调整目标文件大小
    /* set size of output file*/
    if(lseek(fdout,statbuf.st_size-1,SEEK_SET)==-1){
        err_sys("lseek error");
    }
    if(write(fdout," ",1)!=1){ // 写一个空格，形成文件空洞
        err_sys("write error");
    }
    // 建立IO映射
    // 源文件IO映射
    if((src=mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,fdin,0))==MAP_FAILED){ // 映射失败
        err_sys("mmap error for input");
    }
    // 目标文件IO映射
    if((dst=mmap(0,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdout,0))==MAP_FAILED){ // 映射失败
        err_sys("mmap error for output");
    }
    // 内存/文件拷贝
    memcpy(dst,src,statbuf.st_size); /* does the file copy */
    // 退出
    exit(0);
}