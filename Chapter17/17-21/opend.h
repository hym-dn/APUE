#include"../../apue.h"
#include<errno.h>

#define CL_OPEN "open" /* client's request for server */

// 错误消息
extern char errmsg[]; /* error message string to return to client */
// 打开标志
extern int oflag; /* open() flag: O_XXX ... */
// 打开文件路径
extern char *pathname; /* of file t open() for client */

int cli_args(int,char**);
void handle_request(char*,int,int);