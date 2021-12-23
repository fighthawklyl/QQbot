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

        buf = strstr(pstGroup->valuestring, "查豆子");
        if(buf)
        {
            //执行jd脚本读取log文件
        }
        
        

    }
    return ret;
}