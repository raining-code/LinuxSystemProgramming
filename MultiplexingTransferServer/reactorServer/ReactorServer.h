#pragma once
#include "TcpServer.h"
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>
#include <assert.h>
#include <fcntl.h>
#include "Util.h"
#define SIZE_EPOLL 4096
#define SEP "/X"
using std::unordered_map;
using std::vector;
using std::pair;
class ReactorServer;
struct SockEvent {//描述每一个文件描述符的相关信息
    using func_t = void(*)(struct SockEvent*);
    SockEvent(func_t readfunc = nullptr, func_t writefunc = nullptr, func_t errorfunc = nullptr);
    int fd;
    uint32_t events = 0;//需要关心的事件
    string recvMessage;//接受到的数据
    string sendMessage;//要发送的数据
    func_t ReadFunc = nullptr;//将数据读入recvMessage的函数
    func_t WriteFunc = nullptr;//将数据写入sendMessage的函数
    func_t ErrorFunc = nullptr;//处理异常事件的函数
    ReactorServer* reactor = nullptr;
};
class ReactorServer {
public:
    ReactorServer(uint16_t port, int maxQueueLength = 20);
    void ReactorDispatcher(int timeout = -1);//事件派发器
    bool AddSockEvent(int sock, int events); //向事件派发器中添加文件描述符
    void DelSockEvent(int sock);//删除事件派发器中的文件描述符
    int getListenSock();
    int getEpollFd();
private:
    int epfd;
    int listenSock;
public:
    unordered_map<int, SockEvent*> sockMap;//记录epoll模型监视的fd与对应的SockEvent结构的映射
    TcpServer server;
    unordered_map<int, pair<string, uint16_t>> clientMessageSet;//key为fd,value为客户端的ip和端口
};
//链接管理器
void Accepter(SockEvent* sockEvent);

//处理普通文件描述符的读、写异常
void ReadFunc(SockEvent* sockEvent);
void WriteFunc(SockEvent* sockEvent);
void ErrorFunc(SockEvent* sockEvent);