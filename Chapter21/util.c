// 用于两个程序的工具例程

#include "apue.h"
#include "print.h"
#include <ctype.h>
#include <sys/select.h>

// 定义函数所需限制
#define MAXCFGLINE 512 // 打印机配置文件中行的最大长度
#define MAXKWLEN   16 // 配置文件中关键字的最大尺寸
#define MAXFMTLEN  16 // 传给sscanf的格式化字符串的最大长度

/*
 * Get the address list for the given host and service and
 * return through ailistpp.  Returns 0 on success or an error
 * code on failure.  Note that we do not set errno if we
 * encounter an error.
 *
 * LOCKING: none.
 * 将主机名字和服务名字映射到一个地址
 * host - 主机名字
 * service - 服务名字
 * ailistpp - 地址信息存储链表
 * 若成功则返回0，若出错则返回非0错误码
 */
int getaddrlist(const char *host,const char *service,struct addrinfo **ailistpp){
	// 返回值
	int				err;
	// 设置地址过滤模板
	struct addrinfo	hint;
	hint.ai_flags = AI_CANONNAME; // 需要一个规范名称而不是别名
	hint.ai_family = AF_INET; // 地址协议族
	hint.ai_socktype = SOCK_STREAM; // 套接字类型
	hint.ai_protocol = 0; // 协议
	hint.ai_addrlen = 0; // 地址长度
	hint.ai_canonname = NULL; // 规范名称
	hint.ai_addr = NULL; // 地址
	hint.ai_next = NULL; // 链表next指针
	// 将主机名称和服务器名称映射到一个地址
	err = getaddrinfo(host, service, &hint, ailistpp);
	// 返回映射结果
	return(err);
}

/*
 * Given a keyword, scan the configuration file for a match
 * and return the string value corresponding to the keyword.
 *
 * LOCKING: none.
 * 在打印机配置文件中搜索指定的关键字
 */
static char *scan_configfile(char *keyword){
	int				n, match;
	FILE			*fp;
	char			keybuf[MAXKWLEN], pattern[MAXFMTLEN];
	char			line[MAXCFGLINE];
	static char		valbuf[MAXCFGLINE]; // 返回的静态缓冲区
	// 只读打开配置文件
	if ((fp = fopen(CONFIG_FILE, "r")) == NULL)
		log_sys("can't open %s", CONFIG_FILE);
	// 拼接后结果为"%16s,%511s"，可用于格式化字符串，限制输出长度，防止溢出
	sprintf(pattern, "%%%ds %%%ds", MAXKWLEN-1, MAXCFGLINE-1);
	// 循环从文件中读取一行
	match = 0;
	while (fgets(line, MAXCFGLINE, fp) != NULL) {
		// 分解键、值
		n = sscanf(line, pattern, keybuf, valbuf);
		// 如果匹配
		if (n == 2 && strcmp(keyword, keybuf) == 0) {
			match = 1; // 设置标记
			break; // 跳出
		}
	}
	// 关闭文件描述符
	fclose(fp);
	// 返回查找结果
	if (match != 0)
		return(valbuf);
	else
		return(NULL);
}

/*
 * Return the host name running the print server or NULL on error.
 *
 * LOCKING: none.
 * 获取打印假脱机守护进程的计算机系统名称
 */
char *
get_printserver(void)
{
	return(scan_configfile("printserver"));
}

/*
 * Return the address of the network printer or NULL on error.
 *
 * LOCKING: none.
 * 获取网络打印机的地址
 */
struct addrinfo *get_printaddr(void)
{
	// 变量
	int				err;
	char			*p;
	struct addrinfo	*ailist;
	// 获取打印机配置
	if ((p = scan_configfile("printer")) != NULL) {
		// 获取打印机地址
		if ((err = getaddrlist(p, "ipp", &ailist)) != 0) {
			// 出错
			log_msg("no address information for %s", p);
			// 返回
			return(NULL);
		}
		// 返回打印机地址
		return(ailist);
	}
	// 日志
	log_msg("no printer address specified");
	// 返回
	return(NULL);
}

/*
 * "Timed" read - timout specifies the # of seconds to wait before
 * giving up (5th argument to select controls how long to wait for
 * data to be readable).  Returns # of bytes read or -1 on error.
 *
 * LOCKING: none.
 * 读取指定的字节数，在放弃以前至多阻塞timeout秒。当我们从一个套接字或一个管道
 * 读数据时这个函数很有用
 */
ssize_t
tread(int fd, void *buf, size_t nbytes, unsigned int timout)
{
	int				nfds;
	// 描述符集合
	fd_set			readfds;
	// 设置超时时间
	struct timeval	tv;
	tv.tv_sec = timout;
	tv.tv_usec = 0;
	// 异步IO读文件描述符
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	nfds = select(fd+1, &readfds, NULL, NULL, &tv);
	if (nfds <= 0) { // 失败
		// 超时
		if (nfds == 0)
			errno = ETIME;
		// 返回
		return(-1);
	}
	// 读取
	return(read(fd, buf, nbytes));
}

/*
 * "Timed" read - timout specifies the number of seconds to wait
 * per read call before giving up, but read exactly nbytes bytes.
 * Returns number of bytes read or -1 on error.
 *
 * LOCKING: none.
 * tread函数的变体，它只正好读取请求的字节数
 */
ssize_t
treadn(int fd, void *buf, size_t nbytes, unsigned int timout)
{
	size_t	nleft;
	ssize_t	nread;
	// 记录未读字节数
	nleft = nbytes;
	// 如果未读字节数存在
	while (nleft > 0) {
		// 读取失败
		if ((nread = tread(fd, buf, nleft, timout)) < 0) {
			// 未读任何字节
			if (nleft == nbytes)
				return(-1); /* error, return -1 */
			// 已经读部分字节
			else
				break;      /* error, return amount read so far */
		}
		// 读到文件尾
		else if (nread == 0) {
			break;          /* EOF */
		}
		// 计算剩余
		nleft -= nread;
		// 存储读取内容
		buf += nread;
	}
	// 返回未读字节
	return(nbytes - nleft);      /* return >= 0 */
}