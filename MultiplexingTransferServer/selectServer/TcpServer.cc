#include"TcpServer.h"
int TcpServer::Socket() {
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    return listenSock;
}
bool TcpServer::Bind(uint16_t port) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    return bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0;
}
bool TcpServer::Listen(int maxQueueLength) {
    return listen(listenSock, maxQueueLength) == 0;
}
int TcpServer::Accept(string* clientIP, uint16_t* clientPort) {
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t len = sizeof(clientAddr);
    int ret = accept(listenSock, (struct sockaddr*)&clientAddr, &len);
    if (ret > 0) {
        *clientPort = ntohs(clientAddr.sin_port);
        *clientIP = inet_ntoa(clientAddr.sin_addr);
    }
    return ret;
}
void TcpServer::Close() {
    close(listenSock);
}
TcpServer::~TcpServer() {
    Close();
}
