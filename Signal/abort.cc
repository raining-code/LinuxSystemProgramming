#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
using std::cout;
using std::endl;
//SIGABRT信号能否被捕捉?
int main() {
    signal(SIGABRT, [](int signum) {printf("catch %d signal\n", signum);});
    while (1) {
        abort();
        sleep(1);
    }
    return 0;
}
//SIGABRT信号可以被捕捉,通过调用abort函数,永远可以终止进程,即使信号被捕捉
//单独只发送SIGABRT信号,若被捕捉,无法终止进程