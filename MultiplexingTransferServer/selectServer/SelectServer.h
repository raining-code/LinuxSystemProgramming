#pragma once
#include"TcpServer.h"
#include<vector>
#include<unordered_set>
#include<unordered_map>
using std::unordered_map;
using std::pair;
using std::vector;
using std::unordered_set;
class SelectServer {
public:
    SelectServer(uint16_t port, int maxQueueLength = 20);
    void Run();//开始运行服务器
private:
    unordered_set<int> monitorFds;//保存所有需要监视的文件描述符
    int listenSock;//用于监听的文件描述符
    TcpServer server;
    unordered_map<int, pair<string, uint16_t>> clientMessageSet;//key为fd,value为客户端的ip和端口
};