#include"../../apue.h"
#include<errno.h>
#include<limits.h>

static void pr_sysconf(char *,int);
static void pr_pathconf(char *,char *,int);

int main(int argc,char *argv[]){
    //输入参数个数非法
    if(argc!=2)
        err_quit("usage: a.out <dirname>");
    //打印ARG_MAX（exec函数的参数最大长度（字节数））
#ifdef ARG_MAX
    printf("ARG_MAX defined to be %d\n",ARG_MAX+0);
#else
    printf("no symbol for ARG_MAX\n");
#endif
   //运行时获取相应参数的值
#ifdef _SC_ARG_MAX
    pr_sysconf("ARG_MAX=",_SC_ARG_MAX);
#else
    printf("no symbol for _SC_ARG_MAX\n");
#endif
    /* similar processing for all the reset of the sysconf symbols ... */
    //打印MAX_CANON(终端规范输入队列的最大字节数)
#ifdef MAX_CANON
    printf("MAX_CANON defined to be %d\n",MAX_CANON+0);
#else
    printf("no symbol for MAX_CANON\n");
#endif
    //运行时获取相应参数的值
#ifdef _PC_MAX_CANON
    pr_pathconf("MAX_CANON=",argv[1],_PC_MAX_CANON);
#else
    printf("no symbol for _PC_MAX_CANON\n");
#endif
    /* similar processing for all the reset of the sysconf symbols ... */
    exit(0);
}

static void pr_sysconf(char *mesg,int name){
    long val;
    //标准输出上输出消息
    fputs(mesg,stdout);
    //清空错误标记
    errno=0;
    //检测系统运行时配置
    if((val=sysconf(name))<0){
        //错误产生
        if(errno!=0){
            if(errno==EINVAL){//异常name值
                fputs(" (not supported)\n",stdout);
            }else{ // 其他错误
                err_sys("sysconf error");
            }
        }
        //无错误
        else{
            fputs(" (no limit)\n",stdout);
        }
    }else{
        printf(" %d\n",val);
    }
}

static void pr_pathconf(char *mesg,char *path,int name){
    long val;
    //标准输出输出信息
    fputs(mesg,stdout);
    //清空错误
    errno=0;
    //获取指定文件或路径限制
    if((val=pathconf(path,name))<0){
        //如果产生错误
        if(errno!=0){
            if(errno==EINVAL){//输入的name值非法
                fputs(" (not supported)\n",stdout);
            }else{ //其他错误
                err_sys("pathconf error,path=%s",path);
            }
        }
        //无错误
        else{
            fputs(" (no limits\n)",stdout);
        }
    }else{
        printf(" %ld\n",val);
    }
}