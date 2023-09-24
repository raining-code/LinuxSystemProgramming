#pragma once
#include<sys/select.h>
#include<iostream>
#include<string>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
using std::cout;
using std::endl;
using std::string;
class TcpServer {
public:
    int Socket();
    bool Bind(uint16_t port);
    bool Listen(int maxQueueLength = 20);
    int Accept(string* clientIP, uint16_t* clientPort);
    ~TcpServer();
private:
    void Close();
private:
    int listenSock;
};