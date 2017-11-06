#include"../../apue.h"
#include<sys/shm.h>

#define ARRAY_SIZE 40000
#define MALLOC_SIZE 100000
#define SHM_SIZE 100000
#define SHM_MODE 0600

char array[ARRAY_SIZE];

int main(void){
    //共享内存标识符
    int shmid;
    //内存分配首地址
    char *ptr;
    //关联共享内存首地址
    char *shmptr;
    //打印数组的起始、结束地址
    printf("array[] from %lx to %lx\n",
        (unsigned long)&array[0],
        (unsigned long)&array[ARRAY_SIZE]);
    //打印当前共享内存标识符地址
    printf("stack around %lx\n",(unsigned long)
        &shmid);
    //分配内存
    if((ptr=(char *)malloc(MALLOC_SIZE))==NULL){
        err_sys("malloc error");
    }
    //打印内存起始、结束地址
    printf("malloced from %lx to %lx\n",(unsigned long)ptr,
        (unsigned long)ptr+MALLOC_SIZE);
    //创建共享内存
    if((shmid=shmget(IPC_PRIVATE,SHM_SIZE,SHM_MODE))<0){
        err_sys("shmget error");
    }
    //关联共享内存
    if((shmptr=(char *)shmat(shmid,0,0))==(void *)-1){
        err_sys("shmat error");
    }
    //打印共享内存起始、结束地址
    printf("shared memory attached form %lx to %lx\n",
        (unsigned long)shmptr,(unsigned long)shmptr+SHM_SIZE);
    //删除共享内存
    if(shmctl(shmid,IPC_RMID,0)<0){
        err_sys("shmctl error");
    }
    //退出进程
    exit(0);
}
