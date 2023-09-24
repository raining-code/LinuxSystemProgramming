#include "ReactorServer.h"
void ReadFunc(SockEvent* sockEvent) {
    int fd = sockEvent->fd;
    int epfd = sockEvent->reactor->getEpollFd();
    string& recvMessage = sockEvent->recvMessage;
    string& sendMessage = sockEvent->sendMessage;
    string& ip = sockEvent->reactor->clientMessageSet[fd].first;
    uint16_t port = sockEvent->reactor->clientMessageSet[fd].second;
    char buffer[BUFSIZ] = { 0 };
    while (1) {
        ssize_t len = read(fd, buffer, BUFSIZ - 1);
        if (len > 0) {
            buffer[len] = 0;
            recvMessage += buffer;
            printf("从客户端[%s:%d]读取到数据\n", ip.c_str(), port);
        }
        else if (len == 0) {
            printf("已经与客户端[%s:%d]断开连接\n", ip.c_str(), port);
            sockEvent->reactor->DelSockEvent(fd);
            return;
        }
        else {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN) {
                break;
            }
            //表示真的读取出错
            sockEvent->ErrorFunc(sockEvent);
            break;
        }
    }
    //所有读到的内容都是以"/X"作为报文之间的分隔
    auto requests = SplitString(recvMessage, SEP);
    for (string& request : requests) {
        mathcalc::Response response = ConstructResponse(request);
        string responseStr;
        response.SerializePartialToString(&responseStr);
        sendMessage += responseStr;
        sendMessage += SEP;
    }
    //让epoll模型监听写事件，如果就绪，那么这个文件描述符就可以直接写入了
    struct epoll_event events;
    memset(&events, 0, sizeof(events));
    events.events = (EPOLLOUT | EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR);
    events.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &events);
}
void WriteFunc(SockEvent* sockEvent) {
    int fd = sockEvent->fd;
    int epfd = sockEvent->reactor->getEpollFd();
    string& sendMessage = sockEvent->sendMessage;
    while (1) {
        ssize_t len = write(fd, sendMessage.c_str(), sendMessage.size());
        if (len == -1) {
            if (errno == EINTR) {//表示此次调用被信号打断
                continue;
            }
            //EAGIN或写的时候发生了错误，一个都没写进去
            break;
        }
        else {
            if (len == sendMessage.size()) {
                sendMessage.clear();
                //全部写完了，不用再关心写事件了
                struct epoll_event events;
                memset(&events, 0, sizeof(events));
                events.events = (EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR);
                events.data.fd = fd;
                epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &events);
            }
            else {
                sendMessage = sendMessage.substr(len);
            }
        }
    }
}
void ErrorFunc(SockEvent* sockEvent) {//直接断开连接
    int fd = sockEvent->fd;
    int epfd = sockEvent->reactor->getEpollFd();
    string& ip = sockEvent->reactor->clientMessageSet[fd].first;
    uint16_t port = sockEvent->reactor->clientMessageSet[fd].second;
    printf("从%d号文件描述符上接收到异常事件\n", fd);
    printf("已经与客户端[%s:%d]断开连接\n", ip.c_str(), port);
    sockEvent->reactor->DelSockEvent(fd);
}
void Accepter(SockEvent* sockEvent) {
    assert(sockEvent->fd == sockEvent->reactor->getListenSock());
    while (1) {
        //一直获取新链接
        string ip;
        uint16_t port;
        int fd = sockEvent->reactor->server.Accept(&ip, &port);
        if (fd < 0) {
            if (errno == EAGAIN) {
                cout << "底层链接已经全部读取完毕" << endl;
            }
            break;
        }
        printf("客户端[%s:%d]\n", ip.c_str(), port);
        sockEvent->reactor->clientMessageSet.insert(std::make_pair(fd, std::make_pair(ip, port)));
        if (sockEvent->reactor->AddSockEvent(fd, EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR) == false) {
            sockEvent->reactor->clientMessageSet.erase(fd);
            printf("已经与客户端[%s:%d]断开连接\n", ip.c_str(), port);
        }
    }
}
SockEvent::SockEvent(func_t readfunc, func_t writefunc, func_t errorfunc) {
    readfunc = ReadFunc;
    writefunc = WriteFunc;
    errorfunc = ErrorFunc;
}
int ReactorServer::getEpollFd() {
    return epfd;
}
int ReactorServer::getListenSock() {
    return listenSock;
}
ReactorServer::ReactorServer(uint16_t port, int maxQueueLength) {
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
    AddSockEvent(listenSock, EPOLLIN | EPOLLET);
}
void ReactorServer::ReactorDispatcher(int timeout) {
    //事件派发器不断调用epoll_wait函数来获取已经就绪的文件描述符
    vector<struct epoll_event> eventsVec(SIZE_EPOLL);
    while (1) {
    CONTINUE_EPOLL_WAIT:
        int num = epoll_wait(epfd, eventsVec.data(), SIZE_EPOLL, timeout);
        if (num < 0) {
            cout << "此次epoll_wait调用失败" << endl;
            cout << "将发起下一次epoll_wait调用" << endl;
            goto CONTINUE_EPOLL_WAIT;
        }
        if (num == 0) {
            cout << "此次epoll_wait调用超时" << endl;
            cout << "将发起下一次epoll_wait调用" << endl;
            goto CONTINUE_EPOLL_WAIT;
        }
        for (int i = 0;i < num;i++) {//对就绪事件进行处理
            auto it = sockMap.find(eventsVec[i].data.fd);
            assert(it != sockMap.end());
            assert(it->second->fd == it->first);
            if (eventsVec[i].events & EPOLLIN) {//表示读事件就绪
                if (it->second->ReadFunc) {
                    it->second->ReadFunc(it->second);
                }
            }
            if (eventsVec[i].events & EPOLLOUT) {//表示写事件就绪
                if (it->second->WriteFunc) {
                    it->second->WriteFunc(it->second);
                }
            }
            if (eventsVec[i].events & EPOLLERR || eventsVec[i].events & EPOLLHUP) {//表示异常事件就绪
                if (it->second->ErrorFunc) {
                    it->second->ErrorFunc(it->second);
                }
            }
        }
    }
}
bool ReactorServer::AddSockEvent(int sock, int events) { //向事件派发器中添加文件描述符
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = events;
    event.data.fd = sock;
    SetNoBlock(sock);//设置非阻塞
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) < 0) {
        cout << (sock == listenSock ? "将监听套接字添加到epoll模型失败" : "普通套接字添加到epoll模型失败") << endl;
        if (sock == listenSock) {
            exit(0);
        }
        else {
            close(sock);//如果是普通套接字添加到epoll模型失败,直接close
        }
        return false;
    }
    SockEvent* sockEvent = new SockEvent;
    if (sock == listenSock) {
        sockEvent->ReadFunc = Accepter;//仅需设置读回调即可
    }
    else {
        sockEvent->ReadFunc = ReadFunc;
        sockEvent->WriteFunc = WriteFunc;
        sockEvent->ErrorFunc = ErrorFunc;
    }
    sockEvent->fd = sock;
    sockEvent->events = events;
    sockEvent->reactor = this;
    sockMap.insert(std::make_pair(sock, sockEvent));
    return true;
}

void ReactorServer::DelSockEvent(int sock) {//删除事件派发器中的文件描述符
    assert(sock != listenSock && sock != epfd && sock > 0);
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
    sockMap.erase(sock);
    clientMessageSet.erase(sock);
    close(sock);
}