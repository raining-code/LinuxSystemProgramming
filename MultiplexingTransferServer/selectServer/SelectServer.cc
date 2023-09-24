#include"SelectServer.h"
SelectServer::SelectServer(uint16_t port, int maxQueueLength) {
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
    monitorFds.insert(listenSock);//将listenSock加入到监视的文件描述符
}
void SelectServer::Run() {
    //将客户端发送过来的数据进行打印
    while (1) {//循环调用select
        int maxFd = 0;
        fd_set readFdSet;
        FD_ZERO(&readFdSet);
        for (int fd : monitorFds) {
            FD_SET(fd, &readFdSet);
            maxFd = std::max(fd, maxFd);
        }
    CONTINUE_SELECT:
        //调用select阻塞等待多个文件描述符上的读事件是否就绪
        int readyNum = 0;//就绪的文件描述符的个数
        readyNum = select(maxFd + 1, &readFdSet, nullptr, nullptr, nullptr);
        if (readyNum == -1) {
            cout << "此次select调用失败" << endl;
            cout << "将发起下一次select调用" << endl;
            goto CONTINUE_SELECT;
        }
        size_t num = sizeof(fd_set) * 8;
        int check = 0;//检查select函数的返回值是否为就绪的文件描述符个数
        for (int i = 0;i < num;i++) {
            if (FD_ISSET(i, &readFdSet)) {
                check++;
                if (i == listenSock) {
                    cout << "有连接事件就绪" << endl;
                    string ip;
                    uint16_t port = 0;
                    int fd = server.Accept(&ip, &port);
                    if (fd < 0) {
                        cout << "获取连接失败" << endl;
                    }
                    else {
                        cout << "获取链接成功" << endl;
                        printf("客户端[%s:%d]\n", ip.c_str(), port);
                        clientMessageSet.insert(std::make_pair(fd, std::make_pair(ip, port)));
                        monitorFds.insert(fd);
                    }
                }
                else {//有读事件就绪
                    printf("客户端[%s:%d]发送过来数据:\n", clientMessageSet[i].first.c_str(), clientMessageSet[i].second);
                    char buffer[BUFSIZ] = { 0 };
                    ssize_t len = read(i, buffer, BUFSIZ - 1);
                    if (len == 0) {
                        printf("客户端[%s:%d]已经断开连接\n", clientMessageSet[i].first.c_str(), clientMessageSet[i].second);
                        monitorFds.erase(i);
                        clientMessageSet.erase(i);
                        close(i);
                    }
                    else if (len > 0) {
                        buffer[len] = 0;
                        cout << buffer << endl;
                    }
                    else {
                        cout << "read函数读取失败" << endl;
                    }
                }
            }
        }
        if (check != readyNum) {
            cout << "检查select函数的返回值失败" << endl;
            cout << __FILE__ << endl;
            cout << __LINE__ << endl;
        }
    }
}