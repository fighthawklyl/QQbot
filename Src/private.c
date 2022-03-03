#include "private.h"
#include "cJSON.h"
#include "init.h"
#include "tools.h"

extern UserConfig userconfig;
extern JdConfig jdconfig;

int Get_Jdbean(long long qq, char *cmd, char *sendmsg, int size)
{
    int filesize = 0;
    char cmdarr[50];
    char *pBuffer = NULL;
    char *buff = NULL;
    char data[10240];

    for (int i = 0; i < jdconfig.jdlistsize; i++)
    {
        if (jdconfig.jdlist[i].qq == qq)
        {
            buff = jdconfig.jdlist[i].account;
            break;
        }
        if(i == jdconfig.jdlistsize - 1)
        {
            strcpy(sendmsg, "{\"reply\": \"您不存在jd账户\"}");
            return 0;
        }
    }
    memset(cmdarr, 0, 50);
    sprintf(cmdarr, "jd %s now > jd.log", strlen(cmd), cmd);
    system(cmdarr);
    filesize = FileGetSize("./jd.log");
    if (filesize < 0)
    {
        return 1;
    }

    pBuffer = malloc(filesize);
    if (NULL == pBuffer)
    {
        return 1;
    }

    memset(pBuffer, 0, filesize);
    if (FileRead("./jd.log", pBuffer, filesize))
    {
        free(pBuffer);
        pBuffer = NULL;
        return 1;
    }

    memset(data, 0, 10240);
    sprintf(data, "********开始【京东%s", strlen(buff), buff);
    char *str1 = strstr(pBuffer, data);

    char *str2 = strstr(str1 + 1, "********开始【京东");
    int len = str2 - str1 + 1;
    if (len > size)
        return 1;
    
    memset(data, 0, 10240);
    memcpy(data, str1, len);
    sprintf(sendmsg, "{\"reply\": \"%s\"}", data);
    free(pBuffer);
    pBuffer = NULL;
    return 0;
}

int Parse_message(cJSON *pstRoot, char *sendmsg, int size)
{
    cJSON *pstGroup;
    char *buf;
    long long qq = 0;
    int ret = -1;

    pstGroup = cJSON_GetObjectItem(pstRoot, "user_id");
    if (pstGroup)
    {
        qq = pstGroup->valueint;
    }

    pstGroup = cJSON_GetObjectItem(pstRoot, "message");
    if (pstGroup)
    {
        printf("message = %s\n", pstGroup->valuestring);

        if (!Check_Jd_permission(qq))
        {

            //查豆子这个指令从json文件中读取，

            for (int i = 0; i < jdconfig.jdcmdlistsize; i++)
            {
                buf = strstr(pstGroup->valuestring, jdconfig.jdcmdlist[i].cmd);
                if (buf)
                {
                    if (!strcmp(jdconfig.jdcmdlist[i].cmd, "查豆子"))
                    {
                        return Get_Jdbean(qq, jdconfig.jdcmdlist[i].jdcmd, sendmsg, size);
                    }

                    if (!strcmp(jdconfig.jdcmdlist[i].cmd, "更新cookie"))
                    {
                        return -1;
                    }
                    if (!strcmp(jdconfig.jdcmdlist[i].cmd, "添加cookie"))
                    {
                        return -1;
                    }
                }
            }
        }
    }
    return ret;
}