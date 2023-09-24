#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<sys/types.h>
#include<sys/sem.h>
//读进程从共享内存读数据
int main() {
    int key = ftok("/", 666);
    int shmid = shmget(key, 4096, IPC_CREAT);
    char* startAddr = shmat(shmid, NULL, 0);
    if (startAddr == (void*)-1) {
        printf("attach fail\n");
        goto END;
    }
    int semid = semget(key, 1, IPC_CREAT);
    struct sembuf sem_op;
    sem_op.sem_flg = SEM_UNDO;
    char message[4096];
    while (1) {
        sem_op.sem_op = -1;//P操作
        semop(semid, &sem_op, 1);//申请信号量
        strcpy(message, startAddr);
        sem_op.sem_op = 1;//V操作
        semop(semid, &sem_op, 1);//释放信号量
        if (strcmp(message, "quit") == 0) {
            break;
        }
        printf("%s\n", message);
    }
    printf("%s\n", message);
    shmdt(startAddr);
END:
    return 0;
}