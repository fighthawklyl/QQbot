#include "init.h"
#include "server.h"

ServerConfig serverconfig;
ClientConfig clientconfig;
UserConfig userconfig;
JdConfig jdconfig;
Yys_fd yys_fd;

int main()
{
    int ret = 0;
    ret = init_Config();
    if(ret < 0)
    {
        printf("init config error!\n");
        return -1;
    }

    //启动qq机器人线程服务，更具json文件配置启动对应线程
    Start_Server();

    //关闭机器人释放内存

    return 0;
}