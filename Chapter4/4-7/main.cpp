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
#define FTW_F 1 /* file other than directory */ // 文件非目录
#define FTW_D 2 /* directory */ // 目录
#define FTW_DNR 3 /* directory that can't be read */ // 不能读目录
#define FTW_NS 4 /* file that we can't stat */ // 不能获取结构信息的文件

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
    // 遍历目录
    return(dopath(func));
}

/**
 * Descend through the hierarchy,starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(),and return. For a directory,we call ourself
 * recursively for each name in the directory.
 */
static int dopath(Myfunc *func){ /* we return whatever func() returns */
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret;
    char *ptr;
    // 获取文件信息结构
    if(lstat(fullpath,&statbuf)<0){ /* stat error */
        return(func(fullpath,&statbuf,FTW_NS)); // 文件计数
    }
    // 如果当前文件不是目录
    if(S_ISDIR(statbuf.st_mode)==0){ /* not a directory */
        return(func(fullpath,&statbuf,FTW_F)); // 文件计数
    }
    /**
     * It's a directory. First call func() for the 
     * directory,then process each filename in the 
     * directory. 
     */
    // 如果当前文件是目录
    if((ret=func(fullpath,&statbuf,FTW_D))!=0){ // 文件计数失败
        return(ret);
    }
    // 文件计数成功
    // 增加路径分隔符
    ptr=fullpath+strlen(fullpath); /* point to end of fullpath */
    *ptr++='/';
    *ptr=0;
    // 打开目录
    if((dp=opendir(fullpath))==NULL){ /* can't read directory */ // 失败
        return(func(fullpath,&statbuf,FTW_DNR)); // 文件计数
    }
    // 读取目录信息
    while((dirp=readdir(dp))!=NULL){ // 读取成功
        // 如果文件为.或..
        if(strcmp(dirp->d_name,".")==0||
            strcmp(dirp->d_name,"..")==0){
            continue; /* ignore dot and dot-dot */
        }
        // 拼接完成路径名称
        strcpy(ptr,dirp->d_name); /* append name after slash */
        // 递归遍历路径
        if((ret=dopath(func))!=0){ /* recursize */
            break; /* time to leave */
        }
    }
    // ptr指向的上一个元素
    ptr[-1]=0; /* erase everthing from slash onwards */
    // 关闭目录
    if(closedir(dp)<0){
        err_ret("can't close directory %s",fullpath);
    }
    // 返回错误
    return(ret);
}

static int myfunc(const char *pathname,const struct stat *statptr,int type){
    switch(type){ // 类型
        case FTW_F: // 文件
            switch(statptr->st_mode&S_IFMT){
            case S_IFREG: nreg++; break; // 普通文件
            case S_IFBLK: nblk++; break; // 块特殊文件
            case S_IFCHR: nchr++; break; // 字符特殊文件
            case S_IFIFO: nfifo++; break; // 管道或FIFO
            case S_IFLNK: nslink++; break; // 符号连接
            case S_IFSOCK: nsock++; break; // 套接子
            case S_IFDIR: // 目录文件
                err_dump("for S_IFDIR for %s",pathname);
            }
            break;
        case FTW_D: // 目录
            ndir++;
            break;
        case FTW_DNR: // 不能读目录
            err_ret("can't read directory %s",pathname);
            break;
        case FTW_NS: // 获取获取文件结构信息
            err_ret("stat error for %s",pathname);
            break;
        default: // 默认
            err_dump("unknown type %d for pathname %s",type,pathname);
    }
    // 返回
    return(0);
}