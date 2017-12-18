#include"opend.h"
#include<poll.h>

#define NALLOC 10

static struct pollfd *grow_pollfd(struct pollfd *pfd,int *maxfd){
    int i;
    int oldmax=*maxfd;
    int newmax=oldmax+NALLOC;
    //重新分配内存
    if((pfd=realloc(pfd,newmax*sizeof(struct pollfd)))==NULL){
        err_sys("realloc error");
    }
    //初始化重新分配的区域
    for(i=oldmax;i<newmax;i++){
        pfd[i].fd=-1;
        pfd[i].events=POLLIN;
        pfd[i].revents=0;
    }
    //更新最大描述符
    *maxfd=newmax;
    //返回描述符
    return(pfd);
}

void loop(void){
    int i,listenfd,clifd,nread;
    char buf[MAXLINE];
    uid_t uid;
    struct pollfd *pollfd;
    int numfd=1;
    int maxfd=NALLOC;
    //分配描述符集合
    if((pollfd=malloc(NALLOC*sizeof(struct pollfd)))==NULL){
        err_sys("malloc error");
    }
    //初始化描述符集合
    for(i=0;i<NALLOC;i++){
        pollfd[i].fd=-1;
        pollfd[i].events=POLLIN;
        pollfd[i].revents=0;
    }
    //启动监听
    if((listenfd=serv_listen(CS_OPEN))<0){
        log_sys("serv_listen error");
    }
    //添加客户端
    client_add(listenfd,0);
    //向集合中添加监听描述符
    pollfd[0].fd=listenfd;
    //
    for(;;){
        //多路IO复用
        if(poll(pollfd,numfd,-1)<0){
            log_sys("poll error");
        }
        //新客户端连接事件
        if(pollfd[0].revents&POLLIN){
            //接收新客户端
            if((clifd=serv_accept(listenfd,&uid))<0){
                log_sys("serv_accept error: %d",clifd);
            }
            //增加新客户端
            client_add(clifd,uid);
            //如果客户端容量已达上限
            if(numfd==maxfd){
                //扩容
                pollfd=grow_pollfd(pollfd,&maxfd);
            }
            //保存客户端
            pollfd[numfd].fd=clifd;
            pollfd[numfd].events=POLLIN;
            pollfd[numfd].revents=0;
            //描述符数量自加
            ++numfd++;
            //输出日志
            log_msg("new connection: uid %d,fd %d",uid,clifd);
        }
        //
        for(i=1;i<numfd;i++){
            //描述符挂起
            if(pollfd[i].revents&POLLHUP){
                goto hungup;
            }
            //描述符可读
            else if(pollfd[i].revents&POLLIN){
                //从客户端读数据
                if((nread=read(pollfd[i].fd,buf,MAXLINE))<0){
                    log_sys("read error on fd %d",pollfd[i].fd);
                }
                //读为空
                else if(nread==0){
hungup:
                    //日志
                    log_msg("closed: uid %d,fd %d",client[i].uid,
                        pollfd[i].fd);
                    //删除客户端
                    client_del(pollfd[i].fd);
                    //关闭文件描述符
                    close(pollfd[i].fd);
                    //移动最后一个文件描述符到当前位置
                    if(i<(numfd-1)){
                        pollfd[i].fd=pollfd[numfd-1].fd;
                        pollfd[i].events=pollfd[numfd-1].events;
                        pollfd[i].revents=pollfd[numfd-1].revents;
                        i--;
                    }
                    //描述符数量自减
                    numfd--;
                }
                //处理客户端请求
                else{
                    request(buf,nread,pollfd[i].fd,client[i].uid);
                }
            }
        }
    }
}
