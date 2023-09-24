#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
using std::cout;
using std::endl;
int main() {
    signal(SIGCHLD, SIG_IGN);
    if (fork() == 0) {
        exit(0);
    }
    sleep(10);
    return 0;
}