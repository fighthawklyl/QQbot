#ifndef TOOLS_H_
#define TOOLS_H_

//获取文件大小
int FileGetSize(const char *path);

//读文件
int FileRead(const char *path, const char *buf, int count);

//写文件
int FileWrite(const char *path, const char *buf, int count);
#endif