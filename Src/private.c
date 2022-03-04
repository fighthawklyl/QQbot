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
            printf("\33[46;30m __________%s,qq = %d  %s,%d____________\33[0m\n",__FILE__,qq,cmd ,__LINE__);
        if (jdconfig.jdlist[i].qq == qq)
        {
            buff = jdconfig.jdlist[i].account;
            break;
        }
            printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
        if(i == jdconfig.jdlistsize)
        {
            strcpy(sendmsg, "{\"reply\": \"您不存在jd账户\"}");
            printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
            return 0;
        }
    }
    printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
    memset(cmdarr, 0, 50);
    sprintf(cmdarr, "jd %s now > jd.log", cmd);
        printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
//    system(cmdarr);
        printf("\33[46;30m __________%s, %s ,%d____________\33[0m\n",__FILE__, cmdarr,__LINE__);
    filesize = FileGetSize("./jd.log");
    if (filesize < 0)
    {
        return 1;
    }
printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
    pBuffer = malloc(filesize);
    if (NULL == pBuffer)
    {
        return 1;
    }
printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
    memset(pBuffer, 0, filesize);
    if (FileRead("./jd.log", pBuffer, filesize))
    {
        free(pBuffer);
        pBuffer = NULL;
        return 1;
    }
printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
    memset(data, 0, 10240);
    sprintf(data, "********开始【京东%s", buff);
    char *str1 = strstr(pBuffer, data);
    if(str1 == NULL)
    {
        return 1;
    }
    char *str2 = strstr(str1 + 1, "********开始【京东");
    int len = filesize;
    if(str2)
    {
        len = str2 - str1;
    }

    if (len > size)
        return 1;

    memset(data, 0, 10240);
    memcpy(data, str1, len);
 //   printf("\33[46;30m __________%s,%d___%s_________\33[0m\n",__FILE__, __LINE__,data);
    LFtoCRLF(data);

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
            printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
            //查豆子这个指令从json文件中读取，

            for (int i = 0; i < jdconfig.jdcmdlistsize; i++)
            {
                buf = strstr(pstGroup->valuestring, jdconfig.jdcmdlist[i].cmd);
                if (buf)
                {
                    printf("\33[46;30m __________%s,%d____________\33[0m\n",__FILE__, __LINE__);
                    if (!strcmp(jdconfig.jdcmdlist[i].cmd, "查豆子"))
                    {
                        printf("获取豆子数目\n");
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