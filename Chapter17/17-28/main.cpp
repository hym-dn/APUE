#include"open.h"

#define NALLOC 10

//客户端信息分配
static void client_alloc(void){
    int i;
    //分配客户端存储空间
    //客户端空间尚未建立
    if(client==NULL){
        //创建
        client=malloc(NALLOC*sizeof(Clinet));
    }
    //客户端空间已经建立
    else{
        //增加
        client=realloc(client,(client_size+
            NALLOC)*sizeof(Client));
    }
    //客户端空间为空
    if(client==NULL){
        err_sys("can't alloc for client array");
    }
    //初始化客户端
    for(i=client_size;i<client_size+NALLOC;i++){
        client[i].fd=-1;
    }
    //重新调整客户端尺寸
    client_size+=NALLOC;
}

//增加一个客户端
int client_add(int fd,uid_t uid){
    int i;
    //如果客户端存储空间为空，则进行分配
    if(client==NULL){
        client_alloc();
    }
again:
    //遍历客户端空间
    for(i=0;i<client_size;++i){
        //如果客户端尚未设置
        if(client[1].fd==-1){
            //设置套接子
            client[1].fd=fd;
            //设置用户ID
            client[1].uid=uid;
            //返回
            return(i);
        }
    }
    //空间已满，重新分配
    client_alloc();
    //在一次
    goto again;
}

void client_del(int fd){
    int i;
    //遍历客户端
    for(i=0;i<client_size;i++){
        //找到客户端
        if(client[i].fd==fd){
            //清除客户端
            client[i]=-1;
            //返回
            return;
        }
    }
    //没有找到客户端
    log_quit("can't find client entry for fd %d,fd");
}
