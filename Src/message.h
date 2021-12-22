#ifndef MESSAGE_H
#define MESSAGE_H

//解析接收到的json数据属于那种上报类型 分别为message 、notice 。 
//成功返回 0 失败返回 -1
//buf 接收倒的json数据。  sendmsg 返回给上一级调用的回复数据
int Parse_post_type(char* buf, char* sendmsg, int size);

//处理notic类型上报信息 
//根据notice类型 分发给 群相关消息全部由 group.c函数处理
//个人消息由private.c处理
int post_notice_type(cJSON *pstRoot, char* sendmsg, int size);

//出啦request类型消息
int post_request_type(cJSON *pstRoot, char* sendmsg, int size);


//处理message类型上报信息,识别处理私聊消息 或 群聊消息
//分发给 group.c  和 private.c 两个文件处理
int post_message_type(cJSON *pstRoot, char* sendmsg, int size);

#endif