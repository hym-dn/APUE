#include"../../apue.h"
#include<errno.h>

// 服务器全局名字
#define CS_OPEN "/home/sar/opend" /* well-known name */
// 客户端请求命令
#define CL_OPEN "open" /* client's request for server */

extern int debug; /* nonzero if interactive (not daemon) */
// 错误消息
extern char errmsg[]; /* error message string to return to client */
// 文件打开标志
extern int oflag; /* open flag: O_XXX ... */
// 文件路径
extern char *pathname; /* fd,or -1 if available */

// 客户端信息结构体
typedef struct{ /* one Client struct per connected client */
    int fd; /* fd,or -1 if available */
    uid_t uid;
}Client;

extern Client *client; /* ptr to malloc'ed array */
extern int client_size; /* # entries in client[] array */

int cli_args(int,char**);
int client_add(int,uid_t);
void client_del(int);
void loop(void);
void request(char*,int,int,uid_t);