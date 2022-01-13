#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

//获取文件大小
int FileGetSize(const char *path)  
{  
    struct stat statbuff;  

    if (!stat(path, &statbuff)) {  
        return statbuff.st_size;  
    }  else {
        return -1;  
	}
}

//读文件
int FileRead(const char *path, const char *buf, int count)
{
	if (!path || !buf || count<=0)
		return -1;

	int fd = -1;	

	if ((fd=open(path,O_RDONLY)) < 0)
	{
		perror("open");
		return -1;
	}
	
	if (read(fd,buf,count)<=0)
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
	if (!path || !buf || count<=0)
		return -1;

	int fd = -1;	

	if ((fd=open(path,O_WRONLY | O_TRUNC)) < 0)
	{
		perror("open");
		return -1;
	}
	
	if (write(fd,buf,count) != count)
	{
		perror("write");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}