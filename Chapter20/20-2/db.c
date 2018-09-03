#include"../../apue.h"
#include"apue_db.h"
#include<fcntl.h> /* open & db_open flags */ // 文件锁
#include<stdarg.h> // 可变参数函数
#include<errno.h> // 错误编号
#include<sys/uio.h> /* struct iovec */ // 批量IO

/**
 * Internal index file constants.
 * These are used to construct records in the
 * index file and data file.
 */
#define IDXLEN_SZ 4 /* index record length (ASSCII chars) */ // 索引长度
#define SEP ':' /* separator char in index record */ // 分隔符
#define SPACE ' ' /* space character */ // 空格符
#define NEWLINE '\n' /* newline character */ // 换行符

/**
 * The following definitions are hash chains and free
 * list chain in the index file.
 */
#define PTR_SZ 6 /* size of ptr field in hash chain */ // 散列表中指针的长度
#define PTR_MAX 999999 /* max file offset = 10**PTR_SZ-1 */ // 最大文件偏移
#define NHASH_DEF 137 /* default hash table size */ // 默认的哈希表大小
#define FREE_OFF 0 /* free list offfset in index file */ // 索引文件中空闲链表偏移量
#define HASH_OFF PTR_SZ /* hash table offset in index file */ // 索引文件中哈希表偏移量

typedef unsigned long DBHASH; /* hash values */ // 哈希值
typedef unsigned long COUNT; /* unsigned counter */ // 计数

/**
 * Library's private representation of the database.
 */
typedef struct{
    int idxfd; /* fd for index file */ // 索引文件描述符
    int datfd; /* fd for data file */ // 数据文件描述符
    char *idxbuf; /* malloc'ed buffer for index record */ // 索引缓冲区
    char *datbuf; /* malloc'ed buffer for data record */ // 数据缓冲区
    char *name; /* name db was opened under */ // 数据名称
    off_t idxoff; /* offset in index file of index record */ // 索引处于索引文件中的偏移量
                  /* key is at (idxoff + PTR_SZ + IDXLEN_SZ) */
    size_t idxlen; /* length of index record */ // 一条索引长度
                   /* excludes IDXLEN_SZ bytes at front of record */
                   /* includes newline at end of index record */
    off_t datoff; /* offset in data file of data record */ // 数据记录处于数据文件中的偏移量
    size_t datlen; /* length of data record */ // 数据记录的长度
                   /* includes newline at end */
    off_t ptrval; /* contents of chain ptr in index record */ // 索引文件中链表指针内容 ??
    off_t ptroff; /* chain ptr offset pointing to this idx record */ // 链表指针偏移量 ??
    off_t hashoff; /* offset in index file of hash table */ //   索引文件中哈希偏移 ??
    DBHASH nhash; /* current hash table size */ // 当前哈希表尺寸 ??
    COUNT cnt_delok; /* delete OK */ // 删除计数
    COUNT cnt_delerr; /* delete error */ // 删除错误??
    COUNT cnt_fetchok; /* fetch OK */ // 获取计数??
    COUNT cnt_fetcherr; /* fetch error */ // 获取错误??
    COUNT cnt_nextrec; /* nextrec */
    COUNT cnt_stor1; /* store: DB_INSERT,no empty,appended */
    COUNT cnt_stor2; /* store: DB_INSERT,found empty,reused */
    COUNT cnt_stor3; /* store: DB_REPLACE,diff len,appended */
    COUNT cnt_stor4; /* store: DB_REPLACE,same len,overwrote */
    COUNT cnt_storerr; /* store error */
}DB;