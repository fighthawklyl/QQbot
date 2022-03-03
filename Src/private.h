#ifndef PRIVATE_H_
#define PRIVATE_H_
#include "cJSON.h"

//解析message
int Parse_message(cJSON *pstRoot, char* sendmsg, int size);

#endif