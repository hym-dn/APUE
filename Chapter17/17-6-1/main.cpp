#include"../../apue.h"
#include<poll.h>
#include<pthread.h>
#include<sys/msg.h>
#include<sys/socket.h>

//消息队列个数
#define NQ 3
//最大消息尺寸
#define MAXMSZ 512
//消息队列基础主键
#define KEY 0x123

//线程信息
struct threadinfo{
    //消息队列ID
    int qid;
    int fd;
};

//自定义消息
struct mymesg{
    //消息类型
    long mtype;
    //消息数据
    char mtext[MAXMSZ];
};

void *helper(void *arg){
    int n;
    //消息
    struct mymesg m;
    //线程信息
    struct threadinfo *tip=(struct threadinfo*)arg;
    //循环
    for(;;){
        //清空消息
        memset(&m,0,sizeof(m));
        //接收消息
        if((n=msgrcv(tip->qid,&m,MAXMSZ,0,MSG_NOERROR))<0){
            err_sys("msgrcv error");
        }
        //写套接字
        if(write(tip->fd,m.mtext,n)<0){
            err_sys("write error");
        }
    }
}

int main(){
    int i,n,err;
    //UNIX阈报文套接字
    int fd[2];
    //队列ID
    int qid[NQ];
    //poll监听结构体
    struct pollfd pfd[NQ];
    //线程信息
    struct threadinfo ti[NQ];
    //线程ID
    pthread_t tid[NQ];
    //读取缓冲区
    char buf[MAXMSZ];
    //循环建立消息队列
    for(i=0;i<NQ;i++){
        //创建或打开消息队列
        //KEY+i 消息队列主键
        //IPC_CREAT创建
        //0666 用户读写、组读写、其他读写
        //返回消息ID
        if((qid[i]=msgget((KEY+i),IPC_CREAT|0666))<0){
            err_sys("mesgget error");
        }
        //打印队列ID
        printf("queue ID %d is %d\n",i,qid[i]);
        //创建UNIX阈报文套接字
        if(socketpair(AF_UNIX,SOCK_DGRAM,0,fd)<0){
            err_sys("socketpair error");
        }
        //设置poll结构体
        //套接字
        pfd[i].fd=fd[0];
        //事件
        pfd[i].events=POLLIN;
        //设置线程信息
        //队列ID
        ti[i].qid=qid[i];
        //套接字
        ti[i].fd=fd[1];
        //启动线程
        if((err=pthread_create(&tid[i],NULL,helper,&ti[i]))!=0){
            err_exit(err,"pthread_create error");
        }
    }
    //循环接收消息
    for(;;){
        //监听套接字
        if(poll(pfd,NQ,-1)<0){
            err_sys("poll error");
        }
        //循环读取消息队列
        for(i=0;i<NQ;i++){
            //如果读事件触发
            if(pfd[i].revents&POLLIN){
                //读套接字
                if((n=read(pfd[i].fd,buf,sizeof(buf)))<0){
                    err_sys("read error");
                }
                //追加结束符
                buf[n]=0;
                //显示数据
                printf("queue id %d,message %s\n",qid[i],buf);
            }
        }
    }
    //退出
    exit(0);
}
