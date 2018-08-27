#include"../17-27/opend.h"
#include"../../Chapter13/13-1/daemonize.h"
#include<syslog.h>

int debug,oflag,client_size,log_to_stderr;
// 错误消息
char errmsg[MAXLINE];
// 路径名称
char *pathname;
// 客户端数组
Client *client=NULL;

int main(int argc,char *argv[]){
    int c;
    // 打开日志文件
    log_open("open.serv",LOG_PID,LOG_USER);
    // 不期望输出到标准出错
    opterr=0; /* don't want getopt() writing to stderr */
    // 分析参数列表，查看是否存在"-d"选项
    while((c=getopt(argc,argv,"d"))!=EOF){
        switch(c){
        case 'd': /* debug */
            debug=log_to_stderr=1;
            break;
        case '?':
            err_quit("unrecognized option: -c",optopt);
        }
    }
    // 如果不期望以调试运行
    if(debug==0){
        daemonize("opend"); // 创建服务进程
    }
    // 进入循环，检测客户端任务
    loop(); /* never returns */
}