// 从标准输入读取数据,打印到标准输出
// 这个程序还可以通过使用重定向进行文件的复制
// ./a.out < infile.txt > outfile.txt
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFSIZE 4096
int main(void) {
    int n;
    char buf[BUFFSIZE];
    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            printf("write error\n");
        }
    }
    if (n < 0) {
        printf("read error\n");
    }
    return 0;
}