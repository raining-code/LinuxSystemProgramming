#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    int c;
    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            printf("put error\n");
        }
    }
    if (ferror(stdin)) {
        printf("input error\n");
    }
    exit(0);
}