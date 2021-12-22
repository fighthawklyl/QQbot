#include "init.h"


//初始化，检查配置文件，
//存在返回 0 ，不存在从github仓库拉取，拉去成功返回 1，拉取失败返回 -1
int checkfile(void)
{
    return 0;
}

//读取服务器配置，主要配置ip端口等信息 Server.Config
int load_ServerConfig(void)
{
    return 0;
}

//读取主人，和超级管理员列表 User.Config
//成功返回 0 失败返回-1
int load_UserConfig(void)
{
    return 0;
}

//读取群信息配置文件 Group.Config
//成功返回 0 失败返回-1
int load_GroupConfig(void)
{
    return 0;
}

//读取个人信息配置文件 Private.Config
//成功返回 0 失败返回-1
int load_PrivateConfig(void)
{
    return 0;
}

//初始化配置
int init_Config(void)
{
    int res = 0;
    while(checkfile())
    {
        if(3 == res)
        {
            printf("缺失配置文件！\n");
            return -1;
        }
        res++;
    }

    if(load_ServerConfig())
    {
        return -1;
    }

    if(load_GroupConfig())
    {
        return -1;
    }

    if(load_UserConfig())
    {
        return -1;
    }

    if(load_PrivateConfig())
    {
        return -1;
    }

    return 0;
}