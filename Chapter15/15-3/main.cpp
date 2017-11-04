#include"../../apue.h"

//管道描述符
static int pfd1[2],pfd2[2];

//创建'等待'环境
void TELL_WAIT(void){
    //创建管道
    if(pipe(pfd1)<0||pipe(pfd2)<0){
        err_sys("pipe error");
    }
}

//通知父进程
void TELL_PARENT(pid_t pid){
    //写管道
    if(write(pfd2[1],"c",1)!=1){
        err_sys("write error");
    }
}

//等待父进程
void WAIT_PARENT(void){
    //接收字符
    char c;
    //读管道
    if(read(pfd1[0],&c,1)!=1){
        err_sys("read error");
    }
    //未知数据
    if(c!='p'){
        err_quit("WAIT_PARENT: incorrect data");
    }
}

//通知子进程
void TELL_CHILD(pid_t pid){
    //写管道
    if(write(pfd1[1],"p",1)!=1){
        err_sys("write error");
    }
}

//等待子进程
void WAIT_CHILD(void){
    //读字符
    char c;
    //读管道
    if(read(pfd2[0],&c,1)!=1){
        err_sys("read error");
    }
    //未知数据
    if(c!='c'){
        err_quit("WAIT_CHILD: incorrect data");
    }
}
