#include"opend.h"
#include<sys/select.h>

void loop(void){
    int i,n,maxfd,maxi,listenfd,clifd,nread;
    char buf[MAXLINE];
    uid_t uid;
    fd_set rset,allset;
    //清除描述符集
    FD_ZERO(&allset);
    //监听客户端请求
    if((listenfd=serv_listen(CS_OPEN))<0){
        log_sys("serv_listen error");
    }
    //向描述符集中追加描述符
    FD_SET(listenfd,&allset);
    //最大描述符
    maxfd=listenfd;
    //最大索引
    maxi=-1;
    //等待客户端事件
    for(;;){
        //获取描述符集
        rset=allset;
        //监听读描述符集
        if((n=select(maxfd+1,&rset,NULL,NULL,NULL))<0){
            log_sys("select error");
        }
        //检测监听描述符是否被设置
        if(FD_ISSET(listenfd,&rset)){
            //接受新客户端请求
            if((clifd=serv_accept(listenfd,&uid))<0){
                log_sys("serv_accept error: %d",clifd);
            }
            //添加一个客户端
            i=client_add(clifd,uid);
            //存储客户端
            FD_SET(clifd,&allset);
            //更新最大描述符
            if(clifd>maxfd){
                maxfd=clifd;
            }
            //更新最大下标
            if(i>maxi){
                maxi=i;
            }
            //输出日志
            log_msg("new connection: uid %d,fd %d",uid,clifd);
            //继续
            continue;
        }
        //遍历描述符数组
        for(i=0;i<=maxi;i++){
            //获取文件描述符
            if((clifd=client[i].fd)<0){
                continue;
            }
            //如果文件描述符状态被置
            if(FD_ISSET(clifd,&rset)){
                //读取客户端描述符
                if((nread=read(clifd,buf,MAXLINE))<0){
                    log_sys("read error on fd %d",clifd);
                }
                //如果读为空
                else if(nread==0){
                    //日志
                    log_msg("closed: uid %d,fd %d",
                        client[i].uid,clifd);
                    //删除客户端
                    client_del(clifd);
                    //清除描述符集
                    FD_CLR(clifd,&allset);
                    //关闭描述符
                    close(clifd);
                }
                //如果不为空
                else{
                    request(buf,nread,clifd,client[i].uid);
                }
            }
        }

    }
}
