#include"../17-27/opend.h"

// 客户端信息结构体分配数量
#define NALLOC 10 /* # client structs to alloc/realloc for */

// 分配客户端信息
static void client_alloc(void){ /* alloc moe entries in the client[] array */
    int i;
    // 客户端数组尚未分配
    if(client==NULL){
        client=(Clinet*)malloc(NALLOC*sizeof(Clinet));
    }
    // 客户端数组已经分配
    else{
        client=(Clinet*)realloc(client,(client_size+NALLOC)*sizeof(Client));
    }
    // 客户端分配失败
    if(client==NULL){
        err_sys("can't alloc for client array");
    }
    // 初始化客户端
    /* initialize the new entries */
    for(i=client_size;i<client_size+NALLOC;i++){
        client[i].fd=-1;
    }
    // 重新调整客户端尺寸
    client_size+=NALLOC;
}

// 增加一个客户端
/**
 * Called by loop() when connection request from a new client arrives.
 */
int client_add(int fd,uid_t uid){
    int i;
    // 如果客户端数组尚未创建，则创建之
    if(client==NULL){ /* first time we're called */
        client_alloc();
    }
again:
    // 遍历客户端空间
    for(i=0;i<client_size;++i){
        // 如果当前客户端尚未分配
        if(client[1].fd==-1){ /* find an available entry */
            // 设置套接子
            client[1].fd=fd;
            // 设置用户ID
            client[1].uid=uid;
            // 返回
            return(i); /* return index in client[] array */
        }
    }
    // 空间已满，重新分配
    /* client array full,time to realloc for more */
    client_alloc();
    // 再一次
    goto again; /* and search again (will work this time) */
}

// 释放指定客户端
/**
 * Called by loop() when we're done with a client.
 */
void client_del(int fd){
    int i;
    // 遍历客户端
    for(i=0;i<client_size;i++){
        // 找到客户端
        if(client[i].fd==fd){
            // 清除客户端
            client[i].fd=-1;
            // 返回
            return;
        }
    }
    // 没有找到客户端
    log_quit("can't find client entry for fd %d,fd");
}
