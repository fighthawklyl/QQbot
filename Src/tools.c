#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "cJSON.h"
#include "init.h"
#include "server.h"

extern JdConfig jdconfig;
extern UserConfig userconfig;
extern ClientConfig clientconfig;
//获取文件大小
int FileGetSize(const char *path)
{
	struct stat statbuff;

	if (!stat(path, &statbuff))
	{
		return statbuff.st_size;
	}
	else
	{
		return -1;
	}
}

//读文件
int FileRead(const char *path, char *buf, int count)
{
	if (!path || !buf || count <= 0)
		return -1;

	int fd = -1;

	if ((fd = open(path, O_RDONLY)) < 0)
	{
		perror("open");
		return -1;
	}

	if (read(fd, buf, count) <= 0)
	{
		perror("read");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

//写文件
int FileWrite(const char *path, const char *buf, int count)
{
	if (!path || !buf || count <= 0)
		return -1;

	int fd = -1;

	if ((fd = open(path, O_WRONLY | O_TRUNC)) < 0)
	{
		perror("open");
		return -1;
	}

	if (write(fd, buf, count) != count)
	{
		perror("write");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

//数字字符转long数字
int atol(char *ch, long *num)
{
	int size = strlen(ch);
	int temp = 0;
	int ret = 0;
	for (int i = 0; i < size; i++)
	{
		if(ch[0] == '-')
		{
			ret = 1;
			continue;
		}
		if(ch[i] < '0' || ch[i] > '9')
		{
			return -1;
		}
		temp = ch[i] - '0';
		*num *= 10;
		*num += ch[i];
	}
	if(ret) *num = -*num;
	return 0;
}

// long数字转字符串
int ltoa(long num, char *ch, int size)
{
	int ret = 0;
	long temp = 0;
	int len = 0;
	if(num < 0)
	{
		ret = 1;
		ch[0] = '-';
		num = -num;
	}

	temp = num;
	while(temp)
	{
		temp /= temp;
		len++;
	}

	if(ret) len++;
	if(len >= size) return -1;

	while(num && len)
	{
		temp %= num;
		num /= num;
		ch[len -1] = temp + '0';
	}
}

int Check_Jd_permission(long long qq)
{
	if (qq == userconfig.root)
	{
		return 0;
	}

	for (int i = 0; i < userconfig.admin_size; i++)
	{
		if (qq == userconfig.admin[i])
		{
			return 0;
		}
	}
	return -1;
}

int set_private_msg(char* msg, char* data, int len,long long qq)
{
	if(qq < 0 || data == NULL || len == 0 || msg == NULL) return -1;
	sprintf(msg,"POST http://%s:%d/send_private_msg HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\nuser_id=%lld&message=%s\r\n\r\n",clientconfig.host,clientconfig.port,17+len,qq,data);
	return 0;
}