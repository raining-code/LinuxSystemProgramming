#include"EpollServer.h"
EpollServer::EpollServer(uint16_t port, int maxQueueLength) {
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
    epfd = epoll_create(SIZE_EPOLL);
    if (epfd < 0) {
        cout << "创建epoll模型失败" << endl;
        cout << __FILE__ << endl;
        cout << __LINE__ << endl;
    }
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events |= EPOLLIN;//读事件
    event.data.fd = listenSock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenSock, &event) < 0) {
        cout << "将监听套接字添加到epoll模型失败" << endl;
        exit(0);
    }
}
void EpollServer::Run() {
    //将客户端发送过来的数据进行打印
    vector<struct epoll_event> evevtsVec(SIZE_EPOLL);
    while (1) {//循环调用poll
    CONTINUE_EPOLL_WAIT:
        int size = epoll_wait(epfd, evevtsVec.data(), SIZE_EPOLL, -1);
        if (size < 0) {
            cout << "此次epoll_wait调用失败" << endl;
            cout << "将发起下一次epoll_wait调用" << endl;
            goto CONTINUE_EPOLL_WAIT;
        }
        for (size_t i = 0; i < size; i++) {
            int fd = evevtsVec[i].data.fd;
            if (evevtsVec[i].events & EPOLLIN) {
                if (fd == listenSock) {
                    cout << "有连接事件就绪" << endl;
                    string ip;
                    uint16_t port = 0;
                    int sock = server.Accept(&ip, &port);
                    if (sock < 0) {
                        cout << "获取连接失败" << endl;
                    }
                    else {
                        cout << "获取链接成功" << endl;
                        printf("客户端[%s:%d]\n", ip.c_str(), port);
                        clientMessageSet.insert(std::make_pair(sock, std::make_pair(ip, port)));
                        struct epoll_event event;
                        memset(&event, 0, sizeof(event));
                        event.events |= EPOLLIN;//读事件
                        event.data.fd = sock;
                        if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) < 0) {
                            printf("客户端[%s:%d]添加到epoll模型失败\n", ip.c_str(), port);
                            close(sock);
                        }
                    }
                }
                else {
                    printf("客户端[%s:%d]发送过来数据:\n", clientMessageSet[fd].first.c_str(), clientMessageSet[fd].second);
                    char buffer[BUFSIZ] = { 0 };
                    ssize_t len = read(fd, buffer, BUFSIZ - 1);
                    if (len == 0) {
                        printf("客户端[%s:%d]已经断开连接\n", clientMessageSet[fd].first.c_str(), clientMessageSet[fd].second);
                        clientMessageSet.erase(fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                        close(fd);
                    }
                    else if (len > 0) {
                        buffer[len] = 0;
                        cout << buffer << endl;
                        continue;
                    }
                    else {
                        cout << "read函数读取失败" << endl;
                    }
                }
            }
        }
    }
}