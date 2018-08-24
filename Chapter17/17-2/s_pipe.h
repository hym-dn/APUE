#include"../../apue.h"

/**
 * Returns a STREAMS-based pipe,width the two file descriptors
 * returned in fd[0] and fd[1].
 */
int s_pipe(int fd[2]){
    return(pipe(fd));
}