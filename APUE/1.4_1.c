// 列出一个目录中所有的文件名
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    DIR* dir = NULL;
    struct dirent* dire = NULL;
    if (argc != 2) {
        printf("error:please list a directory\n");
        return 0;
    }
    if ((dir = opendir(argv[1])) == NULL) {
        printf("error,can't open %s,it's not a directory\n", argv[1]);
        exit(0);
    }
    while ((dire = readdir(dir)) != NULL) {
        printf("%s\n", dire->d_name);
    }
    closedir(dir);
    return 0;
}