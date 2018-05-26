#include"../../apue.h"
#include"../8-3/pr_exit.h"
#include<sys/times.h>

static void pr_times(clock_t,struct tms *,struct tms *);
static void do_cmd(char *);

int main(int argc,char *argv[]){
    int i;
    // 将标准输出设置为不缓冲
    setbuf(stdout,NULL);
    // 执行输入的每一条命令
    for(i=1;i<argc;i++){
        do_cmd(argv[i]); /* once for each command-line arg */
    }
    // 退出
    exit(0);
}

// 执行并计时
static void do_cmd(char *cmd){ /* execute and time the "cmd" */
    // 进程时间信息
    struct tms tmsstart,tmsend;
    // 开始、结束时间
    clock_t start,end;
    // 执行命令状态
    int status;
    // 打印命令信息
    printf("\ncommand:%s\n",cmd);
    // 记录开始时间
    if((start=times(&tmsstart))==-1){ /* starting value */
        err_sys("times error");
    }
    // 执行命令
    if((status=system(cmd))<0){ /* execute command */
        err_sys("system() error");
    }
    // 记录结束时间
    if((end=times(&tmsend))==-1){ /* ending values */
        err_sys("times error");
    }
    // 打印时间
    pr_times(end-start,&tmsstart,&tmsend);
    // 退出进程
    pr_exit(status);
}

// 打印时间
static void pr_times(clock_t real,struct tms *tmsstart,struct tms *tmsend){
    static long clktck=0;
    // 获取系统记录的每秒中的时钟滴答数
    if(clktck==0){ /* fetch clock ticks per second first time */
        if((clktck=sysconf(_SC_CLK_TCK))<0){
            err_sys("sysconf error");
        }
    }
    // 打印时间
    printf(" real: %7.2f\n",real/(double)clktck);
    printf(" user: %7.2f\n",(tmsend->tms_utime-tmsstart->tms_utime)/(double)clktck);
    printf(" sys: %7.2f\n",(tmsend->tms_stime-tmsstart->tms_stime)/(double)clktck);
    printf(" child user: %7.2f\n",(tmsend->tms_cutime-tmsstart->tms_cutime)/(double)clktck);
    printf(" child sys: %7.2f\n",(tmsend->tms_cstime-tmsstart->tms_cstime)/(double)clktck);
}