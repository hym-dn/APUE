#include"../../apue.h"
#include<sys/socket.h>

/**
 * 利用UNIX域套接字创建管道。
 */
/**
 * Returns a full-duplex "stream" pipe (a UNIX domain socket)
 * with the two file descriptors returned in fd[0] and fd[1]. 
 */
int s_pipe(int fd[2]){
    return(socketpair(AF_UNIX,SOCK_STREAM,0,fd));
}