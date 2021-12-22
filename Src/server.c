#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cJSON.h"
#include "server.h"
#include "message.h"

#define PORT 5701

#define BACKLOG 1
#define MAXRECVLEN 10240
        char data[500]={};
//    strcpy(data,"{\"reply\": \"可恶\"}");
//    strcpy(data,"{\"reply\": \"\"}");

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

int post_message_type(cJSON *pstRoot)
{
    cJSON *pstGroup;
    pstGroup = cJSON_GetObjectItem(pstRoot, "message_type");
    if(pstGroup)
    {
        printf("message_type = %s\n",pstGroup->valuestring);
        if(!strcmp(pstGroup->valuestring,"group"))
        {
            pstGroup = cJSON_GetObjectItem(pstRoot, "user_id");
            if(pstGroup)
            {
                printf("user_id = %ld\n",pstGroup->valueint);
                if(pstGroup->valueint)
                {
                    pstGroup = cJSON_GetObjectItem(pstRoot, "message");
                    if(pstGroup)
                    {
                        char *str = strstr(pstGroup->valuestring, "[CQ:at,qq=940249493]");
                        if(str)
                        {
                            str = strstr(pstGroup->valuestring, "说:");
                            if(str)
                            {
                                printf("msg = %s\n",str);
                                sprintf(data,"{\"reply\": \"[CQ:tts,text=%s]\"}",str+4);
                                return 1;
                            }

                            str = strstr(pstGroup->valuestring, "拍一拍:");
                            if(str)
                            {
                                sprintf(data,"{\"reply\": \"[CQ:poke,qq=%s]\"}",str+10);
                                return 1;
                            }
                        }
                    }
                }
                else
                {
                    pstGroup = cJSON_GetObjectItem(pstRoot, "message");
                    if(pstGroup)
                    {
                        char *str = strstr(pstGroup->valuestring, "[CQ:at,qq=940249493]");
                        if(str)
                        {
                            strcpy(data,"{\"reply\": \"略略略！不听不听！\"}");
                            return 1;                                    
                        }                                      
                    }
                }
            }
        }
        else
        {

        }
    }
}

int post_notice_type()
{

}

int Parse_msg(char* buff)
{
    cJSON *pstRoot;
    cJSON *pstGroup;
    cJSON *pstSender;
    pstRoot = cJSON_Parse(buff);
    if(pstRoot)
    {
        pstGroup = cJSON_GetObjectItem(pstRoot, "post_type");
        if(pstGroup)
        {
            printf("post_type = %s\n",pstGroup->valuestring);
            if(!strcmp(pstGroup->valuestring,"message"))
            {
                
            }
        }
    }
    return -1;
}

int Create_Socket_Link(char* host)
{


    strcpy(data,"{\"reply\": \"\"}");
    char sendText[1024];
    sprintf(sendText,"HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",strlen(data),data);

    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
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
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(host);
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
                printf("%s\n", buf);

                char *str = strstr(buf, "\r\n\r\n{");
                char *str1 = strstr(buf, "}\n");
                int len = str1 - str-3;
                memset(data ,0 ,500);
                memset(sendText ,0 ,1024);
                ret = Parse_msg(str+4);
                printf("str = %c\n", *(str+4));
                printf("str1 = %c\n", *(str1));
                printf("len = %d\n", len);
                printf("%s\n", str+4);
            }else
            {
                close(connectfd);
                break;
            }
            
            if(ret == 1)
            {
            
            }
            else
            {
                strcpy(data,"{\"reply\": \"\"}");
            }
            sprintf(sendText,"HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",strlen(data),data);
            printf("%s\n", sendText);
            send(connectfd,sendText, sizeof(sendText), 0); /* send to the client welcome message */
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    char data[50];
    strcpy(data,"{\"reply\": \"可恶\"}");
    char sendText[1024];
    sprintf(sendText,"HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",strlen(data),data);
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
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
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
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
        
        int iret=-1;
        while(1)
        {
            iret = recv(connectfd, buf, MAXRECVLEN, 0);
            if(iret>0)
            {
                printf("%s\n", buf);
            }else
            {
                close(connectfd);
                break;
            }
            printf("%s\n", sendText);
            send(connectfd,sendText, sizeof(sendText), 0); /* send to the client welcome message */
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}
#endif

int main()
{
    char a[20] = {"127.0.0.1"};
    Create_Socket_Link(a);
}



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

    int sendlen = send(socketfd, buf, strlen(buf), 0);
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
    server.sin_port = htons(serverconfig->cqport);
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