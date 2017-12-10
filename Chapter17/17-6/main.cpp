#include"../../apue.h"
#include<sys/socket.h>

/**
 * 利用UNIX域套接字创建管道。
 */
int s_pipe(int fd[2]){
    return(socketpair(AF_UNIX,SOCK_STREAM,0,fd));
}

