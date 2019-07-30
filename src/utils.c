/*
 * utils.c
 *
 *  Created on: 2018年8月27日
 *      Author: jiajiefeng
 */

#include "head.h"

Record record;
redisContext *context;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //互斥锁

int stdo = 1;

void getv(char *cmd, char *var)
{
	pthread_mutex_lock(&lock);
	redisReply *reply = redisCommand(context, cmd);
	pthread_mutex_unlock(&lock);
	if(reply->type==REDIS_REPLY_NIL)
	{
		logger("ERROR CMD:%s return REDIS_REPLY_NIL\n", cmd);
		strcpy(var, "");
		return;
	}
	strcpy(var, reply->str);
	freeReplyObject(reply);
	logger("%s=%s\n", cmd, var);
}

int geti(char *cmd)
{
	char var[20];
	getv(cmd, var);
	return atoi(var);
}
long long incr(char *cmd)
{
	pthread_mutex_lock(&lock);
	redisReply *reply = redisCommand(context, cmd);
	pthread_mutex_unlock(&lock);
	long long value = reply->integer;
	freeReplyObject(reply);
	logger("%s=%lld \n", cmd, value);
	return value;
}

void setv(char *cmd, char *var)
{
	pthread_mutex_lock(&lock);
	redisReply *reply = redisCommand(context, cmd, var);
	pthread_mutex_unlock(&lock);
	freeReplyObject(reply);
	logger("%s=%s\n", cmd, var);
}
void seti(char *cmd, long long i)
{
	char var[30];
	sprintf(var, "%lld", i);
	setv(cmd, var);
}

void setf(char *cmd, float f)
{
	char var[1024];
	sprintf(var, "%.2f", f);
	setv(cmd, var);
}

int exist(char *cmd)
{
	pthread_mutex_lock(&lock);
	redisReply *reply = redisCommand(context, cmd);
	pthread_mutex_unlock(&lock);
	logger("%s=%lld\n", cmd, reply->integer);
	long long res = reply->integer;
	freeReplyObject(reply);
	return res;
}

/**
 *  返回时间字符串
 */
char *timestr(int isName)
{
	static char st[50];
	time_t timeT = time(NULL);
	char *format = "%Y-%m-%d %H:%M:%S";
	if (isName)
		format = "%Y%m%d%H%M%S";
	strftime(st, sizeof(st), format, localtime(&timeT));
	return st;
}

void initRecord()
{
	char name[100];
	char *t = timestr(1);
	char path[50];
	getv(LOG_DIR, path);
	sprintf(name, "%s/%s.csv", path, t);
	strcpy(record.name, name);
	strcpy(record.path, name);
	record.file = fopen(record.path, "a");
	record.size = 0;
}

void initRedis()
{
	context = redisConnect("127.0.0.1", 6379);
	if (context->err)
	{
		logger("connect redisServer err:%s\n", context->errstr);
		redisFree(context);
	}
	else
	{
		logger("connect redisServer success\n");
	}

}

/**
 * 初始化
 */
void init()
{
	initRedis();
	initRecord();
	init_baord();
	openlog("FCCS", LOG_PID | LOG_CONS, LOG_USER);

	//如果没有UUID就生成一个并保存
	if (!exist(HAS_BOARD_UUID))
	{
		uuid_t uuid;
		char str[36];
		uuid_generate(uuid);
		uuid_unparse(uuid, str);
		setv(SET_BOARD_UUID, str);
	}

	// server();

}

/**
 *  写日志
 */
void wf(char *line)
{
	fprintf(record.file, line);
	record.size += strlen(line);
	if (record.size > 1024000) //1MB大小
	{
		fclose(record.file);
		initRecord();
	}

}

//发送UPD数据
void udp(char *msg)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	char addr[20];
	getv(UDP_ADDR, addr);
	saddr.sin_addr.s_addr = inet_addr(addr);
	saddr.sin_port = htons(geti(UDP_PORT));
	socklen_t len = sizeof(saddr);
	sendto(sock, msg, strlen(msg), 0, (struct sockaddr*) &saddr, len);
	close(sock);
}

/**
 * 日志函数
 */
void logger(char* format, ...)
{
	va_list args;
	if(stdo)
	{
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}

	va_start(args, format);
	vsyslog(LOG_INFO, format, args);
	va_end(args);
}

/**
 *  写入到UDP和文件
 */
void writeOut(char *msg)
{

	logger(msg);
	//判断是否需要发送到UDP
	udp(msg);
	//判断是否需要写入到文件
	wf(msg);
}

/**
 * 作为守护进程启动
 */
void server()
{
	int pid = fork();
	if (pid > 0)
	{
		logger("main pid= %d \n", pid);
		exit(0);
	}
	setsid();
	chdir("/tmp");
	umask(0x022);
	close(STDIN_FILENO);
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

}

