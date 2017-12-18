#include"../../apue.h"

#define CS_OPEN "/tmp/opend.socket"
#define CL_OPEN "open"

extern int debug;
//错误信息
extern char errmsg[];
//文件标志
extern int oflag;
//文件路径
extern char *pathname;

//客户端结构体，描述每一个连接的客户端信息
typedef struct{
    //客户端套接字
    int fd;
    //客户端用户ID
    uid_t uid;
}Clinet;

//客户端数组
extern Clinet *clinet;
//客户端尺寸
extern int clinet_size;

//功能函数
int cli_args(int,char**);
int clinet_add(int,uid_t);
void clinet_del(int);
void loop(void);
void handle_request(char *,int,int,uid_t);
