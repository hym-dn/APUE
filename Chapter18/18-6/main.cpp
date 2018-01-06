#include<sys/stat.h>
#include<dirent.h>
#include<limits.h>
#include<string.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>

//设备驱动信息
struct devdir{
    struct devdir *d_next;
    char *d_name;
};

//设备驱动链表头
static struct devdir *head;
//设备驱动链表尾
static struct devdir *tail;
//路径名称
static char pathname[_POSIX_PATH_MAX+1];

//向链表中添加驱动信息
static void add(char *dirname){
    struct devdir *ddp;
    int len;
    //路径长度
    len=strlen(dirname);
    //过滤.路径以及..路径
    if((dirname[len-1]=='.')&&(dirname[len-2]=='/'||
       (dirname[len-2]=='.'&&dirname[len-3]=='/'))){
        return;
    }
    //过滤"/dev/fd"路径
    if(strcmp(dirname,"dev/fd")==0){
        return;
    }
    //创建驱动信息结构体
    ddp=(struct devdir *)malloc(sizeof(struct devdir));
    if(ddp==NULL){
        return;
    }
    //拷贝路径名称
    ddp->d_name=strdup(dirname);
    if(ddp->d_name==NULL){
        free(ddp);
        return;
    }
    //设置next指针
    ddp->d_next=NULL;
    //如果尾指针为空
    if(tail==NULL){
        head=ddp;
        tail=ddp;
    }
    //如果尾指针非空
    else{
        tail->d_next=ddp;
        tail=ddp;
    }
}

//清除链表
static void cleanup(){
    struct devdir *ddp,*nddp;
    //链表头
    ddp=head;
    //表头存在
    while(ddp!=NULL){
        //一下个
        nddp=ddp->d_next;
        //释放当前名称
        free(ddp->d_name);
        //释放当前结构
        free(ddp);
        //变为下一个
        ddp=nddp;
    }
    //重置链表头、尾
    head=NULL;
    tail=NULL;
}

//搜索路径
static char *searchdir(const char *dirname,struct stat *fdstatp){
    //
    struct stat devstat;
    DIR *dp;
    int devlen;
    struct dirent *dirp;
    //拷贝路径名称
    strcpy(pathname,dirname);
    //打开路径
    if((dp=opendir(dirname))==NULL){
        return(NULL);
    }
    //拼接路径
    strcat(pathname,"/");
    //路径长度
    devlen=strlen(pathname);
    //读取路径
    while((dirp=readdir(dp))!=NULL){
        //形成完整的文件名称
        strncpy(pathname+devlen,dirp->d_name,
            _POSIX_PATH_MAX-devlen);
        //跳过标准输入、标准输出
        if(strcmp(pathname,"/dev/stdin")==0||
           strcmp(pathname,"/dev/stdout")==0||
           strcmp(pathname,"/dev/stderr")==0){
            continue;
        }
        //获取文件属性
        if(stat(pathname,&devstat)<0){
            continue;
        }
        //如果当前是路径
        if(S_ISDIR(devstat.st_mode)){
            add(pathname);
            continue;
        }
        //发现匹配
        if(devstat.st_ino==fdstatp->st_ino&&
           devstat.st_dev==fdstatp->st_dev){
            closedir(dp);
            return(pathname);
        }
    }
    //关闭路径
    closedir(dp);
    //返回空
    return(NULL);
}

//获取指定文件描述符名称
char *ttyname(int fd){
    struct stat fdstat;
    struct devdir *ddp;
    char *rval;
    //不是终端设备
    if(isatty(fd)==0){
        return(NULL);
    }
    //获取文件属性
    if(fstat(fd,&fdstat)<0){
        return(NULL);
    }
    //非字符设备
    if(S_ISCHR(fdstat.st_mode)==0){
        return(NULL);
    }
    //查找指定目录
    rval=searchdir("/dev",&fdstat);
    //如果没有找到
    if(rval==NULL){
        for(ddp=head;ddp!=NULL;ddp=ddp->d_next){
            if((rval=searchdir(ddp->d_name,&fdstat))!=NULL){
                break;
            }
        }
    }
    //清除链表
    cleanup();
    //返回名称
    return(rval);
}
