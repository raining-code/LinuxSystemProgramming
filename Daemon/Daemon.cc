#include"Daemon.h"
void DaemonOriginal() {
    if (fork() > 0) {
        exit(0);
    }
    setsid();
    if (fork() > 0) {
        exit(0);
    }
    //守护进程自成会话,且不能是会话首进程
    chdir("/");
    umask(0);
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}
void MyDaemon(int dir, int close) {
    if (fork() > 0) {
        exit(0);
    }
    setsid();//子进程自成会话
    signal(SIGCHLD, SIG_IGN);
    if (fork() > 0) {
        exit(0);
    }
    //守护进程不是会话首进程
    if (dir == 0) {
        chdir("/");
    }
    if (close == 0) {
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        ::close(fd);
    }
}