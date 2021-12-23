/*
本文件函数用于解析接收到的 json信息，识别接收到的信息类型，根据信息类型种类不同，分发处理。
*/
#include "cJSON.h"
#include "message.h"
#include "group.h"
#include "private.h"
#include "cJSON.h"

//解析接收到的json数据属于那种上报类型 分别为message 、notice 。 
//成功返回 0 失败返回 -1
//buf 接收倒的json数据。  sendmsg 返回给上一级调用的回复数据
int Parse_post_type(char* buf, char* sendmsg, int size)
{
    cJSON *pstRoot;
    cJSON *pstGroup;
    int ret = -1;
    pstRoot = cJSON_Parse(buf);
    if(pstRoot)
    {
        pstGroup = cJSON_GetObjectItem(pstRoot, "post_type");
        if(pstGroup)
        {
            printf("post_type = %s\n",pstGroup->valuestring);
            if(!strcmp(pstGroup->valuestring,"message"))
            {
                ret = post_message_type(pstRoot, sendmsg, size);
            }

            if(!strcmp(pstGroup->valuestring,"notice"))
            {
                ret = post_notice_type(pstRoot, sendmsg, size);
            }

            if(!strcmp(pstGroup->valuestring,"request"))
            {
                ret = post_request_type(pstRoot, sendmsg, size);
            }
        }
    }
    return ret;
}

//处理notic类型上报信息 
//根据notice类型 分发给 群相关消息全部由 group.c函数处理
//个人消息由private.c处理
int post_notice_type(cJSON *pstRoot, char* sendmsg, int size)
{
    cJSON *pstGroup;
    cJSON *pstSender;
    int ret = -1;
    pstGroup = cJSON_GetObjectItem(pstRoot, "notice_type");
    if(pstGroup)
    {
        printf("notice_type = %s\n",pstGroup->valuestring);
        if(!strcmp(pstGroup->valuestring,"group_upload"))   //群文件上传
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"group_admin"))    //群管理员变动
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"group_decrease"))    //群成员减少
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"group_increase"))    //群成员增加
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"group_ban"))    //群禁言
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"group_recall"))    //群消息撤回
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"friend_add"))    //好友添加
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"friend_recall"))    //好友消息撤回
        {
            
        }
        else if(!strcmp(pstGroup->valuestring,"notify"))   
        {
            pstSender = cJSON_GetObjectItem(pstRoot, "sub_type");
            if(pstSender)
            {
                if(!strcmp(pstSender->valuestring,"poke"))  //戳一戳
                {
                    pstSender = cJSON_GetObjectItem(pstRoot, "group_id");
                    if(pstSender)                                         //群内戳一戳
                    {

                    }
                    else                                                    //个人戳一戳
                    {

                    }
                }
                else if(!strcmp(pstSender->valuestring,"lucky_king"))  //群红包运气王
                {

                }
                else if(!strcmp(pstSender->valuestring,"honor"))        //群荣誉变更
                {

                }
            }
            else        
            {

            }
        }
        else if (!strcmp(pstGroup->valuestring,"group_card"))           //群成员名片更新
        {
            
        }
        else if (!strcmp(pstGroup->valuestring,"offline_file"))         //接收到离线文件
        {

        }
        else if (!strcmp(pstGroup->valuestring,"client_status"))        //机器人qq在线状态变更
        {

        }
        else if (!strcmp(pstGroup->valuestring,"essence"))               //精华消息
        {

        }
    }
    return ret;
}

int post_request_type(cJSON *pstRoot, char* sendmsg, int size)
{
    cJSON *pstGroup;
    int ret = -1;
    pstGroup = cJSON_GetObjectItem(pstRoot, "request_type");
    if(pstGroup)
    {
        printf("request_type = %s\n",pstGroup->valuestring);
        if(!strcmp(pstGroup->valuestring,"friend"))             //加好友请求
        {
            
        }

        if(!strcmp(pstGroup->valuestring,"group"))              //加群请求
        {
            
        }
    }
    return ret;
}

//处理message类型上报信息,识别处理私聊消息 或 群聊消息
//分发给 group.c  和 private.c 两个文件处理
int post_message_type(cJSON *pstRoot, char* sendmsg, int size)
{
    cJSON *pstGroup;
    int ret = -1;
    pstGroup = cJSON_GetObjectItem(pstRoot, "message_type");
    if(pstGroup)
    {
        printf("message_type = %s\n",pstGroup->valuestring);
        if(!strcmp(pstGroup->valuestring,"private"))
        {
            
        }

        if(!strcmp(pstGroup->valuestring,"group"))
        {
            
        }
    }
    return ret;
}

