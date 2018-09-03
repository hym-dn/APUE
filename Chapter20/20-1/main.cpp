#include"../../apue.h"
#include"../../apue_db.h"
#include<fcntl.h>

int main(void){
    // 数据库句柄
    DBHANDLE db;
    // 打开数据库失败
    if((db=db_open("db4",O_RDWR|O_CREAT|O_TRUNC,FILE_MODE))==NULL){
        err_sys("db_open error");
    }
    // 向数据库插入数据
    if(db_store(db,"Alpha","data1",DB_INSERT)!=0){
        err_quit("db_store error for alpha");
    }
    if(db_store(db,"beta","Data for beta",DB_INSERT)!=0){
        err_quit("db_store error for beta");
    }
    if(db_store(db,"gamma","record3",DB_INSERT)!=0){
        err_quit("db_store error for gamma");
    }
    // 关闭数据库
    db_close(db);
    // 退出
    exit(0);
}