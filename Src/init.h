//初始化加载配置文件
#ifndef INIT_H_
#define INIT_H_

typedef struct UserConfig
{
    long long root;   //超级管理员qq
    long long *admin; //普通管理员qq  //区别在于有无 添加管理员的权限和 qq群中的高级权限(禁言之类)
    int admin_size;
}UserConfig;

typedef struct JdList
{
    long long qq;
    char account[20];
}JdList;

typedef struct JdCmdList
{
    char *cmd;
    char *jdcmd;
}JdCmdList;

typedef struct JdConfig
{
    JdList* jdlist;
    int jdlistsize;        //京东账户数
    JdCmdList* jdcmdlist;
    int jdcmdlistsize;       //京东命令数
    char* beanpath;
    char* cookiepath;
}JdConfig;


//初始化，检查配置文件，
//存在返回 0 ，不存在从github仓库拉取，拉去成功返回 1，拉取失败返回 -1
int checkfile(void);

//读取服务器配置，主要配置ip端口等信息 Server.Config
int load_ServerConfig(void);

//读取主人，和超级管理员列表 User.Config
//成功返回 0 失败返回-1
int load_UserConfig(void);

//读取群信息配置文件 Group.Config
//成功返回 0 失败返回-1
int load_GroupConfig(void);

//读取个人信息配置文件 Private.Config
//成功返回 0 失败返回-1
int load_PrivateConfig(void);

//读取jd配置信息
int load_JdConfig(void);

//初始化配置
int init_Config(void);

#endif