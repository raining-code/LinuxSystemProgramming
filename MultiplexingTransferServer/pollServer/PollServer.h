#include"TcpServer.h"
#include<vector>
#include<poll.h>
#include<string>
#include<unordered_map>
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;
class PollServer {
public:
    PollServer(uint16_t port, int maxQueueLength = 20);
    void Run();
private:
    int listenSock;//用于监听的文件描述符
    TcpServer server;
    vector<struct pollfd> fds;
    unordered_map<int, pair<string, uint16_t>> clientMessageSet;//key为fd,value为客户端的ip和端口
};
