#include"../../apue.h"
#include<dirent.h>
#include<limits.h>

/* function type that is called for each filename */
typedef int Myfunc(const char *,const struct stat *,int);
static Myfunc myfunc;
static int myftw(char *,Myfunc*);
static int dopath(Myfunc *);

static long nreg,ndir,nblk,nchr,nfifo,nslink,nsock,ntot;

int main(int argc,char *argv[]){
    int ret;
    // 如果参数个数非法        
    if(argc!=2){
        err_quit("usage: ftw <starting-pathname>"); // 错误输出
    }
    // 递归遍历文件夹
    ret=myftw(argv[1],myfunc); /* does it all */
    // 统计文件计数
    ntot=nreg+ndir+nblk+nchr+nfifo+nslink+nsock;
    if(0==ntot){ // 计数为0
        ntot=1; /* avoid divide by 0; print 0 for all counts */
    }
    // 输出各类文件计数
    printf("regular files = %7ld,%5.2f %%\n",nreg,nreg*100.0/ntot);
    printf("directories = %7ld,%5.2f %%\n",ndir,ndir*100.0/ntot);
    printf("block special = %7ld,%5.2f %%\n",nblk,nblk*100.0/ntot);
    printf("char special = %7ld,%5.2f %%\n",nchr,nchr*100.0/ntot);
    printf("FIFOs = %7ld,%5.2f %%\n",nfifo,nfifo*100.0/ntot);
    printf("symbolic links = %7ld,%5.2f %%\n",nslink,nslink*100.0/ntot);
    printf("sockets = %7ld,%5.2f %%\n",nsock,nsock*100.0/ntot);
    // 退出
    exit(ret);
}

/**
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4 /* file that we can't stat */

// 存储路径名称
static char *fullpath; /* contains full pathname for every file */

static int myftw(char *pathname,Myfunc *func){ /* we return whatever func() returns */
    int len;
    // 分配文件路径存储空间
    fullpath=path_alloc(&len); /* malloc's for PATH_MAX+1 bytes */
    // 拷贝路径名称
    strncpy(fullpath,pathname,len); /* protect against */
    // 防止溢出
    fullpath[len-1]=0; /* buffer overrun */
    //
    return(dopath(func));
}