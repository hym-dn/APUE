#include"opend.h"

/**
 * This function is called by buf_args(),which is called by 
 * request().buf_args() has broken up the client's buffer 
 * into an argv[]-style array, which we now process.
 */
int cli_args(int argc,char **argv){
    // 参数不合法
    if(argc!=3||strcmp(argv[0],CL_OPEN)!=0){
        // 错误消息
        strcpy(errmsg,"usage: <pathname><oflag>\n");
        // 返回
        return(-1);
    }
    // 路径
    pathname=argv[1]; /* save ptr to pathname to open */
    // 标志
    oflag=atoi(argv[2]);
    // 返回
    return(0);
}