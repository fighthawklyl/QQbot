#include "private.h"
#include "cJSON.h"

int Parse_message(cJSON *pstRoot, char* sendmsg, int size)
{
    cJSON *pstGroup;
    char *buf;
    int ret = -1;
    pstGroup = cJSON_GetObjectItem(pstRoot, "message");
    if(pstGroup)
    {
        printf("message = %s\n",pstGroup->valuestring);
        
        //查豆子这个指令从json文件中读取，
        buf = strstr(pstGroup->valuestring, "查豆子");
        if(buf)
        {
            //检查该qq用户是否有京东账号存在
            //执行jd脚本读取log文件
        }
        
        buf = strstr(pstGroup->valuestring, "更新cookie");
        if(buf)
        {
            //检查该qq用户是否有京东账号存在
            //执行读写更新cookie文件
        }

        buf = strstr(pstGroup->valuestring, "添加cookie");
        if(buf)
        {
            //检查该qq用户是管理员权限
            //执行读写添加cookie倒cookie文件
        }

    }
    return ret;
}