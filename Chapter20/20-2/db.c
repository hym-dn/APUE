#include"../../apue.h"
#include"apue_db.h"
#include<fcntl.h> /* open & db_open flags */
#include<stdarg.h>
#include<errno.h>
#include<sys/uio.h> /* struct iovec */

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
#define NHASH_DEF 137 /* default hash table size */ // 默认的哈希表尺寸
#define FREE_OFF 0 /* free list offfset in index file */ // 索引文件中空闲链表偏移量
#define HASH_OFF PTR_SZ /* hash table offset in index file */ // 索引文件中哈希表偏移量

typedef unsigned long DBHASH; /* hash values */ // 哈希值
typedef unsigned long COUNT; /* unsigned counter */ // 计数