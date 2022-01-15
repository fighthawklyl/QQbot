#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "init.h"
#include "cJSON.h"
#include "tools.h"
#include "server.h"

#define USERCONIFG_PATH "./config/UserConfig.json"
#define SERVERCONFIG_PATH "./config/ServerConfig.json"
#define PRIVATECONFIG_PATH "./config/PrivateConfig.json"
#define JDCONFIG_PATH "./config/JdConfig.json"
#define GROUPCONFIG_PATH "./config/GroupConfig.json"


extern ServerConfig serverconfig;
extern ClientConfig clientconfig;

UserConfig userconfig;
JdConfig jdconfig;

//初始化，检查配置文件，
//存在返回 0 ，不存在从github仓库拉取，拉去成功返回 1，拉取失败返回 -1
int checkfile(void)
{
    int ret = 0;
    if(access("./config/GroupConfig.json",F_OK))
    {
        ret = -1;
    }

    if(access("./config/JdConfig.json",F_OK))
    {
        ret = -1;
    }

    if(access("./config/PrivateConfig.json",F_OK))
    {
        ret = -1;
    }
    
    if(access("./config/ServerConfig.json",F_OK))
    {
        ret = -1;
    }
    
    if(access("./config/UserConfig.json",F_OK))
    {
        ret = -1;
    }

    return ret;
}

//读取服务器配置，主要配置ip端口等信息 Server.Config
int load_ServerConfig(void)
{
    int filesize = -1;
    char *pBuffer = NULL;
    cJSON *pstRoot = NULL;
    cJSON *pItem = NULL;
    int num = 0;

    filesize = FileGetSize(SERVERCONFIG_PATH)
    if(filesize < 0)
    {
        return -1;
    }

    pBuffer = malloc(filesize);
    if(NULL == pBuffer)
    {
        goto _ret;
    }

    memset(pBuffer, 0, filesize);
    if(FileRead(SERVERCONFIG_PATH, pBuffer, filesize))
    {
        pstRoot = cJSON_Parse(pBuffer);
        if(NULL == pstRoot)
        {
            goto _ret;
        }
        
        memset(serverconfig, 0, sizeof(ServerConfig));
        memset(clientconfig, 0, sizeof(ClientConfig));

        pItem = cJSON_GetObjectItem(pstRoot, "serverhost");
        if(NULL != pItem && pItem->type == cJSON_String)
        {
            memcpy(serverconfig.host, pItem->valuestring, sizeof(pItem->valuestring));
            num |= 0x1;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "serverport");
        if(NULL != pItem && pItem->type == cJSON_Number)
        {
            serverconfig.port = pItem->valueint;
            num |= 0x2;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "yysport");
        if(NULL != pItem && pItem->type == cJSON_Number)
        {
            serverconfig.yysport = pItem->valueint;
            num |= 0x4;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "clienthost");
        if(NULL != pItem && pItem->type == cJSON_String)
        {
            memcpy(clientconfig.host, pItem->valuestring, sizeof(pItem->valuestring));
            num |= 0x8;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "clientport");
        if(NULL != pItem && pItem->type == cJSON_Number)
        {
            clientconfig.port = pItem->valueint;
            num |= 0x10;
        }
    }

_ret:
    if(pBuffer)
    {
        free(pBuffer);
        pBuffer = NULL;
    }

    if(pstRoot)
    {
        cJSON_Delete(pstRoot);
        pstRoot = NULL;
    }

    return num & 0x1B ? 0 : -1;

}

//读取主人，和超级管理员列表 User.Config
//成功返回 0 失败返回-1
int load_UserConfig(void)
{
    int filesize = -1;
    char *pBuffer = NULL;
    cJSON *pstRoot = NULL;
    cJSON *pItem = NULL;
    int num = 0;

    filesize = FileGetSize(USERCONIFG_PATH)
    if(filesize < 0)
    {
        return -1;
    }

    pBuffer = malloc(filesize);
    if(NULL == pBuffer)
    {
        goto _ret;
    }

    memset(pBuffer, 0, filesize);

    if(FileRead(USERCONIFG_PATH, pBuffer, filesize))
    {
        pstRoot = cJSON_Parse(pBuffer);
        if(NULL == pstRoot)
        {
            goto _ret;
        }
        
        memset(userconfig, 0, sizeof(userconfig));

        pItem = cJSON_GetObjectItem(pstRoot, "root");
        if(NULL != pItem && pItem->type == cJSON_Number)
        {
            userconfig.root = pItem->valueint;
            num |= 0x1;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "admin");
        if(NULL != pItem && pItem->type == cJSON_Array)
        {
            int arrsize = cJSON_GetArraySize(pItem);
            if(arrsize)
            {
                userconfig.admin = malloc(arrsize * sizeof(int));
                if(NULL == userconfig.admin)
                {
                    goto _ret;
                }
            }
            
            for(int i = 0; i < arrsize; i++)
            {
                cJSON* temp = cJSON_GetArrayItem(pItem, i);
                if(NUll != temp && temp->type == cJSON_Number)
                {
                    userconfig.admin[i] = temp->valueint;
                }
            }
            num |= 0x2;
        }

    }

_ret:
    if(pBuffer)
    {
        free(pBuffer);
        pBuffer = NULL;
    }

    if(pstRoot)
    {
        cJSON_Delete(pstRoot);
        pstRoot = NULL;
    }

    return num & 0x3 ? 0 : -1;
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

//读取jd配置信息
int load_JdConfig(void)
{
    int filesize = -1;
    char *pBuffer = NULL;
    cJSON *pstRoot = NULL;
    cJSON *pItem = NULL;
    cJSON *ptemp = NULL;
    int num = 0;

    filesize = FileGetSize(JDCONFIG_PATH)
    if(filesize < 0)
    {
        return -1;
    }

    pBuffer = malloc(filesize);
    if(NULL == pBuffer)
    {
        goto _ret;
    }

    memset(pBuffer, 0, filesize);

    if(FileRead(JDCONFIG_PATH, pBuffer, filesize))
    {
        pstRoot = cJSON_Parse(pBuffer);
        if(NULL == pstRoot)
        {
            goto _ret;
        }
        
        memset(jdconfig, 0, sizeof(jdconfig));

        pItem = cJSON_GetObjectItem(pstRoot, "jdlist");
        if(NULL != pItem && pItem->type == cJSON_Array)
        {
            int arrsize = cJSON_GetArraySize(pItem);
            if(arrsize)
            {
                jdconfig.jdlist = malloc(arrsize * sizeof(JdList));
                if(NULL == jdconfig.jdlist)
                {
                    goto _ret;
                }
            }

            for(int i = 0; i < arrsize; i++)
            {
                cJSON* temp = cJSON_GetArrayItem(pItem, i);
                if(NULL != temp && temp->type == cJSON_Object)
                {
                    ptemp = cJSON_GetObjectItem(temp, "qq");
                    if(NULL != pItem && pItem->type == cJSON_Number)
                    {
                        jdconfig.jdlist[i]->qq = ptemp->valueint;
                    }

                    ptemp = cJSON_GetObjectItem(temp, "account");
                    if(NULL != pItem && pItem->type == cJSON_String)
                    {
                        int strsize = strlen(ptemp->valuestring);
                        jdconfig.jdlist[i].account = malloc(strsize + 1);
                        if(NULL == jdconfig.jdlist[i].account)
                        {
                            goto _ret;
                        }

                        memcpy(jdconfig.jdlist[i].account, ptemp->valuestring, strsize);
                    }
                }
            }
            num |= 0x1;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "jdcmdlist");
        if(NULL != pItem && pItem->type == cJSON_Array)
        {
            int arrsize = cJSON_GetArraySize(pItem);
            if(arrsize)
            {
                jdconfig.jdcmdlist = malloc(arrsize * sizeof(JdCmdList));
                if(NULL == jdconfig.jdcmdlist)
                {
                    goto _ret;
                }
            }
            
            for(int i = 0; i < arrsize; i++)
            {
                cJSON* temp = cJSON_GetArrayItem(pItem, i);

                if(NULL != temp && temp->type == cJSON_Object)
                {
                    ptemp = cJSON_GetObjectItem(temp, "cmd");
                    if(NULL != pItem && pItem->type == cJSON_String)
                    {
                        int strsize = strlen(ptemp->valuestring);
                        jdconfig.jdcmdlist[i].cmd = malloc(strsize + 1);
                        if(NULL == jdconfig.jdcmdlist[i].cmd)
                        {
                            goto _ret;
                        }

                        memcpy(dconfig.jdcmdlist[i].cmd, ptemp->valuestring, strsize);
                    }

                    ptemp = cJSON_GetObjectItem(temp, "jdcmd");
                    if(NULL != pItem && pItem->type == cJSON_String)
                    {
                        int strsize = strlen(ptemp->valuestring);
                        jdconfig.jdcmdlist[i].jdcmd = malloc(strsize + 1);
                        if(NULL == jdconfig.jdcmdlist[i].jdcmd)
                        {
                            goto _ret;
                        }

                        memcpy(dconfig.jdcmdlist[i].cmd, ptemp->valuestring, strsize);
                    }  
                }
            }
            num |= 0x2;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "beanpath");
        if(NULL != pItem && pItem->type == cJSON_String)
        {
            int strsize = strlen(pItem->valuestring);
            jdconfig.beanpath = malloc(strsize + 1);
            if(NULL == jdconfig.beanpath)
            {
                goto _ret;
            }

            memcpy(jdconfig.beanpath, pItem, strsize);
            num |= 0x4;
        }

        pItem = cJSON_GetObjectItem(pstRoot, "cookiepath");
        if(NULL != pItem && pItem->type == cJSON_String)
        {
            int strsize = strlen(pItem->valuestring);
            jdconfig.cookiepath = malloc(strsize + 1);
            if(NULL == jdconfig.cookiepath)
            {
                goto _ret;
            }

            memcpy(jdconfig.cookiepath, pItem, strsize);
            num |= 0x8;
        }
    }

_ret:
    if(pBuffer)
    {
        free(pBuffer);
        pBuffer = NULL;
    }

    if(pstRoot)
    {
        cJSON_Delete(pstRoot);
        pstRoot = NULL;
    }

    return num & 0xF ? 0 : -1;
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

    if(load_JdConfig())
    {
        return -1;
    }
    
    return 0;
}