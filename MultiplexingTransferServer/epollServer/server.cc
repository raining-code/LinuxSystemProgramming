#include"EpollServer.h"
int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "输入端口(IP已经使用INADDR_ANY)" << endl;
        exit(0);
    }
    EpollServer server(atoi(argv[1]));
    server.Run();
    return 0;
}