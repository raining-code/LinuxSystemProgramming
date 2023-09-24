#include"PollServer.h"
PollServer::PollServer(uint16_t port, int maxQueueLength) {
    listenSock = server.Socket();
    if (listenSock < 0) {
        cout << "创建套接字失败" << endl;
        cout << __FILE__ << endl;
        cout << __LINE__ << endl;
        exit(0);
    }
    if (server.Bind(port) == false) {
        cout << "绑定地址结构失败" << endl;
        cout << __FILE__ << endl;
        cout << __LINE__ << endl;
        exit(0);
    }
    if (server.Listen(maxQueueLength) == false) {
        cout << "监听失败" << endl;
        cout << __FILE__ << endl;
        cout << __LINE__ << endl;
        exit(0);
    }
    struct pollfd PollMessage;
    memset(&PollMessage, 0, sizeof(PollMessage));
    PollMessage.fd = listenSock;
    PollMessage.events |= POLLIN;//读事件
    fds.push_back(PollMessage);
}
void PollServer::Run() {
    //将客户端发送过来的数据进行打印
    while (1) {//循环调用poll
        int readyNum = 0;//就绪的文件描述符的个数
    CONTINUE_POLL:
        readyNum = poll(fds.data(), fds.size(), -1);
        if (readyNum == -1) {
            cout << "此次poll调用失败" << endl;
            cout << "将发起下一次poll调用" << endl;
            goto CONTINUE_POLL;
        }
        int check = 0;//检查poll函数的返回值是否为就绪的文件描述符个数
        for (auto it = fds.begin();it != fds.end();) {
            if (it->revents & POLLIN) {//返回时通过revents得知那些文件描述符上已经有事件就绪
                check++;
                if (it->fd == listenSock) {
                    cout << "有连接事件就绪" << endl;
                    string ip;
                    uint16_t port = 0;
                    int fd = server.Accept(&ip, &port);
                    if (fd < 0) {
                        cout << "获取连接失败" << endl;
                        it++;
                        continue;
                    }
                    else {
                        cout << "获取链接成功" << endl;
                        printf("客户端[%s:%d]\n", ip.c_str(), port);
                        clientMessageSet.insert(std::make_pair(fd, std::make_pair(ip, port)));
                        struct pollfd PollMessage;
                        memset(&PollMessage, 0, sizeof(PollMessage));
                        PollMessage.fd = fd;
                        PollMessage.events |= POLLIN;//读事件
                        it = fds.insert(it, PollMessage);//防止迭代器失效
                        it += 2;
                        continue;
                    }
                }
                else {//有读事件就绪
                    printf("客户端[%s:%d]发送过来数据:\n", clientMessageSet[it->fd].first.c_str(), clientMessageSet[it->fd].second);
                    char buffer[BUFSIZ] = { 0 };
                    ssize_t len = read(it->fd, buffer, BUFSIZ - 1);
                    if (len == 0) {
                        printf("客户端[%s:%d]已经断开连接\n", clientMessageSet[it->fd].first.c_str(), clientMessageSet[it->fd].second);
                        clientMessageSet.erase(it->fd);
                        close(it->fd);//先进行close在erase
                        it = fds.erase(it);
                        continue;
                    }
                    else if (len > 0) {
                        buffer[len] = 0;
                        cout << buffer << endl;
                        it++;
                        continue;
                    }
                    else {
                        cout << "read函数读取失败" << endl;
                        it++;
                        continue;
                    }
                }
            }
            it++;
        }
        if (check != readyNum) {
            cout << "check=" << check << ",readyNum=" << readyNum << endl;
            cout << "检查poll函数的返回值失败" << endl;
            cout << __FILE__ << endl;
            cout << __LINE__ << endl;
        }
    }
}