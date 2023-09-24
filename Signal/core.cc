#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
using std::cout;
using std::endl;
//演示子进程发生核心转储
void JudgeStatus(int status) {
    if (WIFEXITED(status)) {
        printf("child exit normal\n");
        printf("child exit code is %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        printf("child killed by signal:%d\n", WTERMSIG(status));
        if (WCOREDUMP(status)) {
            printf("child coredump\n");
        }
    }
    else {
        printf("Unknown error\n");
    }
}
int main() {
    if (fork() == 0) {
        int* p = nullptr;
        *p = 100;
        exit(0);
    }
    int status;
    wait(&status);
    JudgeStatus(status);
    return 0;
}