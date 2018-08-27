#include"../17-27/opend.h"
#include<sys/select.h>

void loop(void){
    int i,n,maxfd,maxi,listenfd,clifd,nread;
    char buf[MAXLINE];
    uid_t uid;
    fd_set rset,allset;
    // 清除描述符
    FD_ZERO(&allset);
    // 监听客户端请求
    /* obtain fd to listen for client requests on */
    if((listenfd=serv_listen(CS_OPEN))<0){
        log_sys("serv_listen error");
    }
    // 将监听文件描述符，加入文件描述符集
    FD_SET(listenfd,&allset);
    // 更新最大描述符
    maxfd=listenfd;
    // 更新最大下标
    maxi=-1; /* 不必特别记录监听描述符的下标 */
    // 循环等待客户端连接
    for(;;){
        // 重置文件描述符集
        rset=allset; /* rset gets modified each time around s*/
        // 监听读描述符集（等待描述符状态变化）
        if((n=select(maxfd+1,&rset,NULL,NULL,NULL))<0){
            log_sys("select error");
        }
        // 如果监听文集描述符被设置
        if(FD_ISSET(listenfd,&rset)){
            // 等待客户端连接请求
            /* accept new client request */
            if((clifd=serv_accept(listenfd,&uid))<0){
                log_sys("serv_accept error: %d",clifd);
            }
            // 添加一个客户端
            i=client_add(clifd,uid);
            // 存储客户端到描述符集中
            FD_SET(clifd,&allset);
            // 更新最大描述符的值
            if(clifd>maxfd){
                maxfd=clifd; /* max fd for select() */
            }
            // 更新最大下标的值
            if(i>maxi){
                maxi=i; /* max index in client[] array */
            }
            // 输出日志
            log_msg("new connection: uid %d,fd %d",uid,clifd);
            // 继续
            continue;
        }
        // 遍历描述符集合
        for(i=0;i<=maxi;i++){ /* go through client[] array */
            // 获取文件描述符
            if((clifd=client[i].fd)<0){
                continue;
            }
            // 如果文件描述符状态被设置
            if(FD_ISSET(clifd,&rset)){
                // 读取客户端传递过来的参数
                /* read argument buffer from client */
                if((nread=read(clifd,buf,MAXLINE))<0){ // 失败
                    log_sys("read error on fd %d",clifd);
                }
                // 读到客户端断开
                else if(nread==0){ // 读到文件尾
                    // 日志
                    log_msg("closed: uid %d,fd %d",
                        client[i].uid,clifd);
                    // 删除客户端
                    client_del(clifd); /* client has closed cxn */
                    // 清除描述符集
                    FD_CLR(clifd,&allset);
                    // 关闭描述符
                    close(clifd);
                }
                // 从客户端读取成功
                else{ /* process client's request */
                    request(buf,nread,clifd,client[i].uid); // 打开、发送描述符
                }
            }
        }

    }
}
