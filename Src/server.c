#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "cJSON.h"
#include "server.h"
#include "message.h"

#define BACKLOG 5
#define MAXRECVLEN 10240
extern Yys_fd yys_fd;
extern ClientConfig clientconfig;
extern ServerConfig serverconfig;
static pthread_mutex_t g_mutexyys = PTHREAD_MUTEX_INITIALIZER;

pthread_t yysclient_tid[10] = {};

typedef struct Private_msg
{
    int font;
    char message[10240];
    int message_id;
    char message_type[10];
    char post_type[10];
    char raw_message[10240];
    long self_id;
    char sub_type[15];
    long time;
    long user_id;
}Private_msg;

int send_request_msg(char *sendbuf, char *recvbuf, int recvsize)
{
    int socketfd, listenfd;              /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    /* Create TCP socket */
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() error. Failed to initiate a socket\n");
        return -1;
    }

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(clientconfig.port);
    server.sin_addr.s_addr = inet_addr(clientconfig.host);

    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)))
    {
        perror("connection fail!\n");
        close(socketfd);
        return -1;
    }

    int sendlen = send(socketfd, sendbuf, strlen(sendbuf), 0);
    if (sendlen <= 0)
    {
        printf("send fail\n");
        close(socketfd);
        return -1;
    }
    int recvlen = recv(socketfd, recvbuf, recvsize, 0);
    close(socketfd);
    return recvlen;
}

void Create_CQ_Server(void *args)
{
    char sendText[10240];
    char data[10240];
    char lenth[10];
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
printf("---1----\n");
printf("host:%s   port:%d \n",serverconfig.host, serverconfig.port);
    if (serverconfig.port == -1)
    {
        return;
    }
printf("---2----\n");
    /* Create TCP socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* handle exception */
        perror("socket() error. Failed to initiate a socket");
        exit(1);
    }

    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(serverconfig.port);
    server.sin_addr.s_addr = inet_addr(serverconfig.host);
    //  server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);
    while (1)
    {
        if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1)
        {
            perror("accept() error. \n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n", inet_ntoa(client.sin_addr), htons(client.sin_port), tv.tv_sec, tv.tv_usec);
        int ret = -1;
        int iret = -1;
        while (1)
        {
            memset(buf, 0, MAXRECVLEN);
            memset(data, 0, 10240);
            memset(sendText, 0, 10240);
            iret = recv(connectfd, buf, MAXRECVLEN, 0);
            if (iret > 0)
            {
                memset(lenth, 0, 10);
                printf("%s\n", buf);
                char *strlen1 = strstr(buf, "Content-Length: ");
                char *strlen2 = strstr(buf, "Content-Type:");
                int size = strlen2 - strlen1 - 18;
                strncpy(lenth, strlen1 + 16, size);
                printf("size = %d, lenth = %s\n",size,lenth);
                char *str = strstr(buf, "\r\n\r\n{");
                char *str1 = strstr(buf, "}\n");
                int len = str1 - str - 3;
                ret = Parse_post_type(str, data, 10240);
                printf("ret = %d ,data = %s,lenth = %s \n",ret,data,lenth);
                if ((len + 1) != atoi(lenth))
                    ret = -1;
                printf("str = %c\n", *(str + 4));
                printf("str1 = %c\n", *(str1));
                printf("len = %d\n", len);
                printf("%s\n", str + 4);
            }
            else
            {
                close(connectfd);
                break;
            }

            if (ret == -1)
            {
                strcpy(data, "{\"reply\": \"\"}");
            }
            else if(ret == 1)
            {
                strcpy(data, "{\"reply\": \"????????????\"}");
            }

            sprintf(sendText, "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n", strlen(data), data);
            printf("%s\n", sendText);
            send(connectfd, sendText, strlen(sendText), 0); /* send to the client welcome message */
            close(connectfd);
            break;
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}

int Yys_msg_send(int qq, char *data, int size)
{
    for (int i = 0; i < 10; i++)
    {
        if (yys_fd.qq[i] == qq && yys_fd.fd[i] > 0)
        {
            pthread_mutex_lock(&g_mutexyys);
            send(yys_fd.fd[i], data, size, 0);
            pthread_mutex_unlock(&g_mutexyys);
            return 0;
        }
    }
    return -1;
}

int Yys_jpg_recv(cJSON *pstRoot, int index)
{
    cJSON *pItem = NULL;
    cJSON *pRoot = NULL;
    char *jpgname = NULL;
    char jpg_path[100] = {0};
    fd_set Readfd;
    int fd;
    char data[50] = {0};
    int datalen = 0;
    int size = 0;
    int len = 0;
    int iret = -1;
    char buf[MAXRECVLEN];
    struct timeval TimeOut;

    pItem = cJSON_GetObjectItem(pstRoot, "jpg_sign");
    if (NULL != pItem && pItem->type == cJSON_Number)
    {
        if (pItem->valueint == 0)
        {
            pItem = cJSON_GetObjectItem(pstRoot, "jpgname");
            if (NULL != pItem && pItem->type == cJSON_String)
            {
                jpgname = pItem->valuestring;
                strcpy(jpg_path, "./jpg");
                memcpy(jpg_path + 6, jpgname, strlen(jpgname));
                fd = open(jpg_path, O_WRONLY | O_TRUNC);
                if (fd)
                {
                    datalen = strlen("{\"reply\":\"ok\"}");
                    memcpy(data, &datalen, sizeof(datalen));
                    memcpy(data + sizeof(datalen), "{\"reply\":\"ok\"}", datalen);

                    while (1)
                    {
                        TimeOut.tv_sec = 0;
                        TimeOut.tv_usec = 1000;
                        FD_ZERO(&Readfd);
                        FD_SET(yys_fd.fd[index], &Readfd);
                        iret = select(yys_fd.fd[index] + 1, &Readfd, NULL, NULL, &TimeOut);
                        if (iret)
                        {
                            memset(buf, 0, MAXRECVLEN);
                            iret = recv(yys_fd.fd[index], buf, MAXRECVLEN, 0);
                            if (iret <= 0)
                            {
                                close(yys_fd.fd[index]);
                                yys_fd.fd[index] = -1;
                                yys_fd.qq[index] = -1;
                                goto _ret;
                            }

                            memcpy((void *)&size, buf, 4);
                            len = strlen(buf + 4);
                            if (size == len)
                            {
                                pRoot = cJSON_Parse(buf + 4);
                                if (pRoot)
                                {
                                    pItem = cJSON_GetObjectItem(pRoot, "type");
                                    if (NULL != pItem && pItem->type == cJSON_String)
                                    {
                                        if (0 == strcpy(pItem->valuestring, "jpg"))
                                        {
                                            pItem = cJSON_GetObjectItem(pRoot, "jpgname");
                                            if (NULL != pItem && pItem->type == cJSON_String)
                                            {
                                                if (0 == strcpy(pItem->valuestring, jpgname))
                                                {
                                                    pItem = cJSON_GetObjectItem(pRoot, "jpg_sign");
                                                    if (NULL != pItem && pItem->type == cJSON_Number)
                                                    {
                                                        int sign = pItem->valueint;
                                                        if (sign == 0)
                                                        {
                                                            goto _ret;
                                                        }
                                                        else if (sign == 1 || sign == 2)
                                                        {
                                                            pItem = cJSON_GetObjectItem(pRoot, "jpg");
                                                            if (NULL != pItem && pItem->type == cJSON_String)
                                                            {
                                                                write(fd, pItem->valuestring, strlen(pItem->valuestring));
                                                                if (sign == 2)
                                                                {
                                                                    if (pRoot)
                                                                    {
                                                                        free(pRoot);
                                                                        pRoot = NULL;
                                                                    }
                                                                    close(fd);
                                                                    return 0;
                                                                }
                                                                send(yys_fd.fd[index], data, 4 + datalen, 0);
                                                            }
                                                        }
                                                        else
                                                        {
                                                            goto _ret;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (pRoot)
                                {
                                    free(pRoot);
                                    pRoot = NULL;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
_ret:
    if (pRoot)
    {
        free(pRoot);
        pRoot = NULL;
    }
    close(fd);
    return -1;
}

//??????
void YYS_Server(void *args)
{
    char buf[1024];
    char data[100];
    int iret = -1;
    int ret = -1;
    int size = 0;
    int len = 0;
    int datalen = 0;
    fd_set Readfd;
    cJSON *pstRoot = NULL;
    cJSON *pItem = NULL;
    struct timeval TimeOut;
    while (1)
    {

        for (int i = 0; i < 10; i++)
        {
            pthread_mutex_lock(&g_mutexyys);
            if (yys_fd.fd[i])
            {
                TimeOut.tv_sec = 0;
                TimeOut.tv_usec = 1000;
                FD_ZERO(&Readfd);
                FD_SET(yys_fd.fd[i], &Readfd);
                iret = select(yys_fd.fd[i] + 1, &Readfd, NULL, NULL, &TimeOut);
                if (iret > 0)
                {
                    memset(buf, 0, 1024);
                    memset(data, 0, 100);

                    datalen = strlen("{\"reply\":\"fail\"}");
                    memcpy(data, &datalen, sizeof(datalen));
                    memcpy(data + sizeof(datalen), "{\"reply\":\"fail\"}", datalen);

                    iret = recv(yys_fd.fd[i], buf, MAXRECVLEN, 0);
                    if (iret <= 0)
                    {
                        close(yys_fd.fd[i]);
                        yys_fd.fd[i] = -1;
                        yys_fd.qq[i] = -1;
                        send(yys_fd.fd[i], data, 4 + datalen, 0);
                        usleep(10);
                        pthread_mutex_unlock(&g_mutexyys);
                        continue;
                    }

                    memcpy((void *)&size, buf, 4);
                    len = strlen(buf + 4);
                    if (size == len)
                    {
                        pstRoot = cJSON_Parse(buf + 4);
                        if (pstRoot)
                        {
                            pItem = cJSON_GetObjectItem(pstRoot, "type");
                            if (NULL != pItem && pItem->type == cJSON_String)
                            {
                                if (0 == strcpy(pItem->valuestring, "heart"))
                                {
                                    pItem = cJSON_GetObjectItem(pstRoot, "qq");
                                    if (NULL != pItem && pItem->type == cJSON_Number)
                                    {
                                        yys_fd.qq[i] = pItem->valueint;
                                        ret = 0;
                                    }
                                }
                                else if (0 == strcpy(pItem->valuestring, "msg"))
                                {
                                    ret = 0;
                                }
                                else if (0 == strcpy(pItem->valuestring, "jpg"))
                                {
                                    ret = Yys_jpg_recv(pstRoot, i);
                                    //?????????????????????????????????qq,??????????????????json??????
                                    //Yys_msg_send(int qq, char *data, int size);
                                }
                            }
                        }
                        if (pstRoot)
                        {
                            free(pstRoot);
                            pstRoot = NULL;
                            pItem = NULL;
                        }
                    }
                    if (!ret)
                    {
                        datalen = strlen("{\"reply\":\"ok\"}");
                        memcpy(data, &datalen, sizeof(datalen));
                        memcpy(data + sizeof(datalen), "{\"reply\":\"ok\"}", datalen);
                        ret = -1;
                    }
                    send(yys_fd.fd[i], data, 4 + datalen, 0);
                }
            }
            pthread_mutex_unlock(&g_mutexyys);
            usleep(1000);
        }
    }
}

void Create_YYS_Server(void *args)
{
    char sendText[1024];
    char lenth[10];
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
    cJSON *pstRoot = NULL;
    cJSON *pItem = NULL;

    if (serverconfig.yysport == -1)
    {
        return;
    }

    /* Create TCP socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* handle exception */
        perror("socket() error. Failed to initiate a socket");
        exit(1);
    }

    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(serverconfig.yysport);
    server.sin_addr.s_addr = inet_addr(serverconfig.host);
    //  server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);
    for (int i = 0; i < 10; i++)
    {
        yys_fd.fd[i] = -1;
        yys_fd.qq[i] = -1;
    }
    while (1)
    {
        if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1)
        {
            perror("accept() error. \n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n", inet_ntoa(client.sin_addr), htons(client.sin_port), tv.tv_sec, tv.tv_usec);
        for (int i = 0; i < 10; i++)
        {
            if (yys_fd.fd[i] == -1)
            {
                memset(buf, 0, MAXRECVLEN);
                int iret = recv(connectfd, buf, MAXRECVLEN, 0);
                if (iret <= 0)
                {
                    close(connectfd);
                    break;
                }
                int size = 0;
                memcpy((void *)&size, buf, 4);
                int len = strlen(buf + 4);
                if (size == len)
                {
                    pstRoot = cJSON_Parse(buf + 4);
                    if (pstRoot)
                    {
                        pItem = cJSON_GetObjectItem(pstRoot, "type");
                        if (NULL != pItem && pItem->type == cJSON_String)
                        {
                            if (0 == strcpy(pItem->valuestring, "heart"))
                            {
                                pItem = cJSON_GetObjectItem(pstRoot, "qq");
                                if (NULL != pItem && pItem->type == cJSON_Number)
                                {
                                    yys_fd.qq[i] = pItem->valueint;
                                }
                            }
                        }
                    }
                    free(pstRoot);
                    pstRoot = NULL;
                    pItem = NULL;
                }
                yys_fd.fd[i] = connectfd;
            }
            else
            {
                (i == 10) && close(connectfd);
            }
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}

//??????qq???????????????
int Start_Server(void)
{
    pthread_t cqserver_tid = -1;
    pthread_t yysserver_tid = -1;
    pthread_t yysheart_tid = -1;

    // CQ ??????http???????????????
    int err = pthread_create(&cqserver_tid, NULL, (void *)&Create_CQ_Server, NULL);
    if (err)
    {
        perror(err); return -1;
    }

    //??????????????????
    err = pthread_create(&yysserver_tid, NULL, (void *)&Create_YYS_Server, NULL);
    if (err)
    {
        perror(err); return -1;
    }

    // yys????????????????????????????????????qq??????
    err = pthread_create(&yysheart_tid, NULL, (void *)&YYS_Server, NULL);
    if (err)
    {
        perror(err); return -1;
    }
    while(1)
    {
        printf("-------\n");
        sleep(10);
    }
}