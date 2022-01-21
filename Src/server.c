#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

static pthread_mutex_t g_mutexyys=PTHREAD_MUTEX_INITIALIZER;
char data[500]={};
//    strcpy(data,"{\"reply\": \"可恶\"}");
//    strcpy(data,"{\"reply\": \"\"}");
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
};

int send_request_msg(ClientConfig* clientconfig, char* sendbuf, char* recvbuf, int recvsize)
{
    int socketfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    socklen_t addrlen;
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
    server.sin_port = htons(clientconfig->port);
    server.sin_addr.s_addr = inet_addr(clientconfig->host);

    if(connect(socketfd, (struct sockaddr *)&server, sizeof(server)))
    {
        perror("connection fail!\n");
        close(socketfd);
        return -1;
    }

    int sendlen = send(socketfd, sendbuf, strlen(sendbuf), 0);
    if(sendlen <= 0)
    {
        printf("send fail\n");
        close(socketfd);
        return -1;
    }
    int recvlen = recv(socketfd, recvbuf, recvsize, 0);
    close(socketfd);
    return recvlen;
}


void Create_CQ_Server(void* args)
{
    char sendText[1024];
    char lenth[10];
    ServerConfig* serverconfig = (ServerConfig*)args;
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;

    if(serverconfig->port == -1)
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
    server.sin_port = htons(serverconfig->port);
    server.sin_addr.s_addr = inet_addr(serverconfig->host);
//  server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }
    
    if(listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);
    while(1){
        if((connectfd=accept(listenfd,(struct sockaddr *)&client, &addrlen))==-1)
        {
            perror("accept() error. \n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n",inet_ntoa(client.sin_addr),htons(client.sin_port), tv.tv_sec,tv.tv_usec);
        int ret = -1;
        int iret=-1;
        while(1)
        {
            memset(buf,0,MAXRECVLEN);
            iret = recv(connectfd, buf, MAXRECVLEN, 0);
            if(iret>0)
            {
                memset(lenth, 0, 10);
                printf("%s\n", buf);
                char *strlen1 = strstr(buf,"Content-Length: ");
                char *strlen2 = strstr(buf, "Content-Type:");
                int size = strlen2 - strlen1 - 18;
                strncpy(lenth, strlen + 16, size);
                char *str = strstr(buf, "\r\n\r\n{");
                char *str1 = strstr(buf, "}\n");
                int len = str1 - str-3;
                ret = Parse_post_type(str, data , 1024);
                if((len+1) != atoi(lenth)) ret = -1;
                printf("str = %c\n", *(str+4));
                printf("str1 = %c\n", *(str1));
                printf("len = %d\n", len);
                printf("%s\n", str+4);
            }
            else
            {
                close(connectfd);
                break;
            }
            
            if(ret)
            {
                strcpy(data,"{\"reply\": \"\"}");
            }

            sprintf(sendText,"HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",strlen(data),data);
            printf("%s\n", sendText);
            send(connectfd,sendText, sizeof(sendText), 0); /* send to the client welcome message */
            close(connectfd);
            break;
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}

int Yys_msg_send(int qq, char* data, int size)
{
    for(int i = 0; i < 10; i++)
    {
        if(yys_fd.fd[i] == qq)
        {
            pthread_mutex_lock(g_mutexyys);
            send(yys_fd.fd[i], data, size, 0);
            pthread_mutex_unlock(g_mutexyys);
        }
    }
}

//心跳
void YYS_Server(void* args)
{
    char buf[1024];
    int iret = -1;
    fd_set Readfd;
    struct timeval TimeOut;
    while(1)
    {

        for(int i = 0; i < 10; i++)
        {
            pthread_mutex_lock(g_mutexyys);
            if(yys_fd.fd[i])
            {
                TimeOut.tv_sec  = 0;
                TimeOut.tv_usec = 1000;
                FD_ZERO(&Readfd);
                FD_SET(yys_fd.fd[i], &Readfd);
                iret = select(yys_fd.fd[i] + 1, &Readfd, NULL, NULL, &TimeOut);
                if(iret)
                {
                    int iret = recv(yys_fd.fd[i], buf, MAXRECVLEN, 0);
                    if(iret <= 0)
                    {
                        close(yys_fd.fd[i]);
                        yys_fd.fd[i] = -1;
                        pthread_mutex_unlock(g_mutexyys);
                        break;
                    }
                    int size = 0;
                    memcpy((void*)&size, buf, 4);
                    int len = strlen(buf+4);
                    if(size == len)
                    {
                        char* str = strstr(buf+4, "qq:");
                        yys_fd.qq[i] = atoi(str+3);
                    }
                    send(yys_fd.fd[i], "{\"reply\":\"OK\"}", 0, 0);
                }
            }
            pthread_mutex_unlock(g_mutexyys);
            usleep(1000);
        }
        sleep(10);
    }
}

void Create_YYS_Server(void* args)
{
    char sendText[1024];
    char lenth[10];
    ServerConfig* serverconfig = (ServerConfig*)args;
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;

    if(serverconfig->yysport == -1)
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
    server.sin_port = htons(serverconfig->yysport);
    server.sin_addr.s_addr = inet_addr(serverconfig->host);
//  server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }
    
    if(listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);
    for(int i =0;i < 10; i++)
    {
        yys_fd.fd[i] = -1;
        yys_fd.qq[i] = -1;
    }
    while(1){
        if((connectfd=accept(listenfd,(struct sockaddr *)&client, &addrlen))==-1)
        {
            perror("accept() error. \n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n",inet_ntoa(client.sin_addr),htons(client.sin_port), tv.tv_sec,tv.tv_usec);
        for(int i =0;i < 10; i++)
        {
            if(yys_fd.fd[i] == -1)
            {
                memset(buf,0,MAXRECVLEN);
                int iret = recv(connectfd, buf, MAXRECVLEN, 0);
                if(iret <= 0)
                {
                    close(connectfd);
                    break;
                }
                int size = 0;
                memcpy((void*)&size, buf, 4);
                int len = strlen(buf+4);
                yys_fd.fd[i] = connectfd;
                if(size == len)
                {
                    char* str = strstr(buf+4, "qq:");
                    yys_fd.qq[i] = atoi(str+3);
                }
            }
            else
            {
                (i == 9) && close(connectfd);
            }
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}

//启动qq机器人服务
int Start_Server(void)
{
    pthread_t cqserver_tid = -1;
    pthread_t yysserver_tid = -1;
    pthread_t yysheart_tid = -1;

    //CQ 反向http代理服务器
    int err = pthread_create(&cqserver_tid, NULL, (void *)&Create_CQ_Server, NULL);
    if(err)
    {
        perror("%s\n",err)
        return -1;
    }
    
    //阴阳师服务器
    err = pthread_create(&yysserver_tid, NULL, (void *)&Create_YYS_Server, NULL);
    if(err)
    {
        perror("%s\n",err)
        return -1;
    }
    
    //yys心跳，以及脚本和用户通过qq关联
    err = pthread_create(&yysheart_tid, NULL, (void *)&YYS_Server, NULL);
    if(err)
    {
        perror("%s\n",err)
        return -1;
    }
    
}