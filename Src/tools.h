#ifndef TOOLS_H_
#define TOOLS_H_

//获取文件大小
int FileGetSize(const char *path);

//读文件
int FileRead(const char *path, char *buf, int count);

//写文件
int FileWrite(const char *path, const char *buf, int count);

//数字字符转long数字 成功返回0 失败返回-1；
int atol(char *ch, long *num);

// long数字转字符串
int ltoa(long num, char *ch, int size);

//检查京东账户
int Check_Jd_permission(int qq);
#endif