#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
    printf("user ID:%d, user group ID:%d\n", getuid(), getgid());
    return 0;
}