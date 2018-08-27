#include"../17-27/opend.h"
#include<poll.h>

// 监听描述符数组曾长尺寸
#define NALLOC 10 /* # pollfd structs to alloc/realloc */

// 文件描述符存储区扩容
static struct pollfd *grow_pollfd(struct pollfd *pfd,int *maxfd){
    int i;
    int oldmax=*maxfd;
    int newmax=oldmax+NALLOC;
    // 内存扩容
    if((pfd=(struct pollfd*)realloc(pfd,newmax*sizeof(struct pollfd)))==NULL){
        err_sys("realloc error");
    }
    // 初始化重新分配的区域
    for(i=oldmax;i<newmax;i++){
        pfd[i].fd=-1;
        pfd[i].events=POLLIN;
        pfd[i].revents=0;
    }
    // 更新最大描述符
    *maxfd=newmax;
    // 返回描述符
    return(pfd);
}

void loop(void){
    int i,listenfd,clifd,nread;
    char buf[MAXLINE];
    uid_t uid;
    struct pollfd *pollfd;
    int numfd=1; /* 监听文集描述符数量 */
    int maxfd=NALLOC;
    // 分配文件描述符存储数组，并初始化
    if((pollfd=(struct pollfd *)malloc(
        NALLOC*sizeof(struct pollfd)))==NULL){
        err_sys("malloc error");
    }
    for(i=0;i<NALLOC;i++){
        pollfd[i].fd=-1;
        pollfd[i].events=POLLIN;
        pollfd[i].revents=0;
    }
    // 启动监听
    /* obtain of listen for client requests on */
    if((listenfd=serv_listen(CS_OPEN))<0){
        log_sys("serv_listen error");
    }
    // 添加监听文件描述符
    client_add(listenfd,0); /* we use [0] for listenfd */
    // 将文件描述符添加到监听集合中
    pollfd[0].fd=listenfd;
    // 不断循环，等待客户端请求
    for(;;){
        // 监听文件描述符
        if(poll(pollfd,numfd,-1)<0){
            log_sys("poll error");
        }
        // 监听文件描述符可读（客户端连接事件）
        if(pollfd[0].revents&POLLIN){
            // 接收新客户端请求
            /* accept new client request */
            if((clifd=serv_accept(listenfd,&uid))<0){
                log_sys("serv_accept error: %d",clifd);
            }
            // 增加新客户端
            client_add(clifd,uid);
            // 如果客户端容量已达上限
            /* possibly increase the size of the polled array */
            if(numfd==maxfd){
                // 扩容
                pollfd=grow_pollfd(pollfd,&maxfd);
            }
            // 保存客户端
            pollfd[numfd].fd=clifd;
            pollfd[numfd].events=POLLIN;
            pollfd[numfd].revents=0;
            // 描述符数量自加
            numfd++;
            // 输出日志
            log_msg("new connection: uid %d,fd %d",uid,clifd);
        }
        // 监听到客户端事件
        for(i=1;i<numfd;i++){
            // 挂起
            if(pollfd[i].revents&POLLHUP){
                goto hungup;
            }
            // 可读
            else if(pollfd[i].revents&POLLIN){
                // 从客户端读
                /* read argument buffer from client */
                if((nread=read(pollfd[i].fd,buf,MAXLINE))<0){ // 失败
                    log_sys("read error on fd %d",pollfd[i].fd);
                }
                // 读到文件尾
                else if(nread==0){
hungup:
                    /* the client closed the connection */
                    // 日志
                    log_msg("closed: uid %d,fd %d",client[i].uid,
                        pollfd[i].fd);
                    // 删除客户端
                    client_del(pollfd[i].fd);
                    // 关闭文件描述符
                    close(pollfd[i].fd);
                    // 移动最后一个文件描述符到当前位置
                    if(i<(numfd-1)){
                        /* pack the array */
                        pollfd[i].fd=pollfd[numfd-1].fd;
                        pollfd[i].events=pollfd[numfd-1].events;
                        pollfd[i].revents=pollfd[numfd-1].revents;
                        i--; /* recheck this entry */
                    }
                    // 描述符数量自减
                    numfd--;
                }
                // 处理客户端请求
                else{ /* process client's request */
                    request(buf,nread,pollfd[i].fd,client[i].uid);
                }
            }
        }
    }
}