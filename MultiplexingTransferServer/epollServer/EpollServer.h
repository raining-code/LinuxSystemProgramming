#include"TcpServer.h"
#include<vector>
#include<sys/epoll.h>
#include<string>
#include<unordered_map>
#define SIZE_EPOLL 4096//epoll_create函数的参数
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;
class EpollServer {
public:
    EpollServer(uint16_t port, int maxQueueLength = 20);
    void Run();
private:
    int epfd;//epoll模型
    int listenSock;//用于监听的文件描述符
    TcpServer server;
    unordered_map<int, pair<string, uint16_t>> clientMessageSet;//key为fd,value为客户端的ip和端口
};
