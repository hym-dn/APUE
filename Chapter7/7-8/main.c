#include"../../apue.h"
#include<sys/resource.h>

#if defined(BSD)||defined(MACOS)
#include<sys/time.h>
#define FMT "%10lld "
#else
#define FMT "%10ld "
#endif

#define doit(name) pr_limits(#name,name)

static void pr_limits(char *,int);

// 主函数
int main(void){
    // 进程可用存储区的最大总长度（字节）。
#ifdef RLIMIT_AS
    doit(RLIMIT_AS);
#endif
    // core文件的最大字节数，若其值为0则
    // 阻止创建core文件
    doit(RLIMIT_CORE);
    // CPU时间的最大量值（秒）
    doit(RLIMIT_CPU);
    // 数据段的最大字节长度
    doit(RLIMIT_DATA);
    // 可以创建文件的最大字节长度
    doit(RLIMIT_FSIZE);
    // 一个进程可持有的文件锁的最大数
#ifdef RLIMIT_LOCKS
    doit(RLIMIT_LOCKS)
#endif
    // 一个进程使用mlock能够锁定在存储器中
    // 的最大字节长度
#ifdef RLIMIT_MEMLOCK
    doit(RLIMIT_MEMLOCK);
#endif
    // 每个进程能打开的最大文件数
    doit(RLIMIT_NOFILE);
    // 每个实际用户ID可拥有的最大子进程数
#ifdef RLIMIT_NPROC
    doit(RLIMIT_NPROC);
#endif
    // 最大驻内存集的字节长度
#ifdef RLIMIT_RSS
    doit(RLIMIT_RSS);
#endif
    // 用户在任一给定时刻可以占用的
    // 套接子缓冲区的最大长度
#ifdef RLIMIT_SBSIZE
    doit(RLIMIT_SBSIZE);
#endif
    // 栈的最大字节长度
    doit(RLIMIT_STACK);
    // 只是RLMIT_AS的同义词
#ifdef RLIMIT_VMEM
    doit(RLIMIT_VMEM);
#endif
    // 退出
    exit(0);
}

// 打印资源限制
static void pr_limits(char *name,int resource){
    struct rlimit limit;
    // 获取指定进程资源限制
    if(getrlimit(resource,&limit)<0){
        err_sys("getrlimit error for %s",name);
    }
    // 打印资源名称
    printf("%-14s ",name);
    // 如果不限制软件资源
    if(limit.rlim_cur==RLIM_INFINITY){
        printf("(infinite) ");
    }
    // 打印软件资源限制
    else{
        printf(FMT,limit.rlim_cur);
    }
    // 如果不限制硬件资源
    if(limit.rlim_max==RLIM_INFINITY){
        printf("(infinite)");
    }
    // 打印硬件资源限制
    else{
        printf(FMT,limit.rlim_max);
    }
    // 输出回车
    putchar((int)'\n');
}