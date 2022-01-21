#ifndef SERVER_H_
#define SERVER_H_

//server监听的cq服务器的地址和端口,yys的端口
typedef struct ServerConfig
{
    char host[20];
    int port;
    int yysport;
}ServerConfig;

//向cq发送http请求的ip和端口
typedef struct ClientConfig
{
    char host[20];
    int port;
}ClientConfig;

typedef struct Yys_fd 
{
    int qq[10];
    int fd[10];
}Yys_fd;

//向cq发送http请求，并接收返回内容。sendbuf要发送的请求，recvbuf用于接收返回的数据。返回值：成功返回recvbuf接收到的数据长度
//失败返回 -1；
int send_request_msg(ClientConfig* clientconfig, char* sendbuf, char* recvbuf, int recvsize);

//创建cq server线程
void Create_CQ_Server(void* args);

//创建yys服务端线程
void Create_YYS_Server(void* args);

int Start_Server(void);
#endif