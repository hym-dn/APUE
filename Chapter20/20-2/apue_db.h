#ifndef _APUE_DB_H
#define _APUE_DB_H

typedef void *DBHANDLE; // 数据库句柄

DBHANDLE db_open(const char *,int,...); // 打开数据库
void db_close(DBHANDLE); // 关闭数据库
char *db_fetch(DBHANDLE,const char *); // 从数据库中取数据
int db_store(DBHANDLE,const char *,const char *,int); // 向数据库存储数据
int db_delete(DBHANDLE,const char *); // 从数据库删除数据
void db_rewind(DBHANDLE); // 回滚数据
char *db_nextrec(DBHANDLE,char *); // 获取数据库下一条数据

/**
 * Flags for db_store().
 */
#define DB_INSERT 1 /* insert new record only */ // 插入
#define DB_REPLACE 2 /* replace existing record */ // 更新
#define DB_STORE 3 /* replace or insert */ // 插入 or 更新

/**
 * Implementation limits
 */
#define IDXLEN_MIN 6 /* key,sep,start,sep,length,\n */  // 索引最小长度
#define IDXLEN_MAX 1024 /* arbitrary */ // 索引最大长度
#define DATLEN_MIN 2 /* data byte,new line */ // 最小数据长度
#define DATLEN_MAX 1024 /* arbitrary */ // 最大数据长度

#endif /* _APUE_DB_H */