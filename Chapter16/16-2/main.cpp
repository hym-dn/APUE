#include"../../apue.h"
#include<sys/socket.h>

#define MAXSLEEP 128

int connect_retry(int sockfd,const struct sockaddr *addr,socklen_t alen){
    //定义秒
    int nsec;
    //指数补偿(每循环一次增加每次尝试的延迟，
    //直到最大延迟为2分钟)
    for(nsec=1;nsec<=MAXSLEEP;nsec<<=1){
        //连接指定服务器
        if(connect(sockfd,addr,alen)==0){
            return(0);
        }
        //延时重试
        if(nsec<=MAXSLEEP/2){
            sleep(nsec);
        }
    }
    //返回
    return(-1);
}
