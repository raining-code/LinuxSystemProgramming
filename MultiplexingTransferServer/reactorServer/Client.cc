#include "mathcalc.pb.h"
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <random>
#include "Util.h"
#define SEP "/X"
using std::string;
using std::cout;
using std::cin;
using std::endl;
string ConstructRequest() {
    string requestAll;//保存所有的请求
    //随机生成10000个request
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 100000);
    int num = 10000;
    static string ops[] = { "+","-","*","/","!" };
    while (num--) {
        int left = dis(gen);
        int right = dis(gen);
        string op = ops[dis(gen) % 5];
        mathcalc::Request req;
        req.set_leftnum(left);
        req.set_rightnum(right);
        req.set_op(op);
        cout << left << op << right << endl;
        string temp;
        req.SerializePartialToString(&temp);
        requestAll += temp;
        requestAll += SEP;
    }
    cout << endl;
    return requestAll;
}
void SendMessage(int sock, string& sendMessage) {
    if (sendMessage.empty()) {
        return;
    }
    while (1) {
        ssize_t len = write(sock, sendMessage.c_str(), sendMessage.size());
        if (len == -1) {
            if (errno == EINTR) {//表示此次调用被信号打断
                continue;
            }
            //底层发送缓冲区已经满了
            break;
        }
        else {
            if (len == sendMessage.size()) {
                sendMessage.clear();
                return;
            }
            sendMessage = sendMessage.substr(len);
        }
    }
}
vector<mathcalc::Response> ReadMessage(int sock, string& recvMessage) {
    vector<mathcalc::Response> ans;
    char buffer[BUFSIZ] = { 0 };
    while (true) {
        ssize_t len = read(sock, buffer, BUFSIZ - 1);
        if (len > 0) {
            buffer[len] = 0;
            recvMessage += buffer;
        }
        else {
            break;
        }
    }
    //所有读到的内容都是以"/X"作为报文之间的分隔
    auto responses = SplitString(recvMessage, SEP);
    for (string& response : responses) {
        mathcalc::Response res;
        res.ParseFromString(response);
        ans.push_back(res);
    }
    return ans;
}
int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "请输入服务器的IP和端口" << endl;
        exit(0);
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "socket error" << endl;
        exit(0);
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "connect error" << endl;
        exit(0);
    }
    SetNoBlock(sock);
    string sendMessage;
    string recvMessage;
    auto requests = ConstructRequest();
    while (true) {
        SendMessage(sock, requests);
        auto ans = ReadMessage(sock, recvMessage);
        for (auto& response : ans) {
            if (response.flag() == false) {
                cout << response.leftnum() << response.op() << response.rightnum() << "不合法" << endl;
            }
            else {
                cout << response.leftnum() << response.op() << response.rightnum() << "=" << response.ans() << endl;
            }
        }
    }
    return 0;
}