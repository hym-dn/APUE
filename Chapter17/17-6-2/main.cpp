#include"../../apue.h"
#include<sys/msg.h>

//最大消息尺寸
#define MAXMSZ 512

//消息结构体
struct mymesg{
    //消息类型
    long mtype;
    //消息数据
    char mtext[MAXMSZ];
};

int main(int argc,char *argv[]){
    //消息队列主键
    key_t key;
    //消息队列ID
    long qid;
    //字节数
    size_t nbytes;
    //消息
    struct mymesg m;
    //输入参数非法
    if(argc!=3){
        fprintf(stderr,"usage: sendmsg KEY message\n");
    }
    //获取消息队列主键
    key=strtol(argv[1],NULL,0);
    //打开消息队列
    if((qid=msgget(key,0))<0){
        err_sys("can't open queue key %s",argv[1]);
    }
    //初始化消息
    memset(&m,0,sizeof(m));
    //填充消息
    strncpy(m.mtext,argv[2],MAXMSZ-1);
    //计算字节数
    nbytes=strlen(m.mtext);
    //消息类型
    m.mtype=1;
    //发送消息
    if(msgsnd(qid,&m,nbytes,0)<0){
        err_sys("can't send message");
    }
    //退出
    exit(0);
}

