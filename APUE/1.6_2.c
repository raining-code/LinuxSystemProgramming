#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAXLINE 1000
int main() {
    char buf[MAXLINE];
    pid_t pid;
    int status;
    printf("%% "); // 格式控制
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = 0;
        }
        if ((pid = fork()) < 0) {
            printf("create child process error\n");
        } else if (pid == 0) {
            // 子进程
            execlp(buf, buf, NULL);
            // 进程程序替换失败
            printf("could not excute: %s\n", buf);
            exit(127);
        } else { // 父进程返回子进程的pid
            if ((pid = waitpid(pid, &status, 0)) < 0) {
                printf("waitpid error\n");
            }
            printf("%% ");
        }
    }
    return 0;
}