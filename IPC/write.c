#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/wait.h>
//写进程向共享内存写数据
int main() {
    int fd = open("./out.txt", O_WRONLY);
    dup2(fd, stdout->_fileno);
    close(fd);
    int key = ftok("/", 666);
    int shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid < 0) {
        printf("create shared memory fail\n");
        exit(0);
    }
    char* startAddr = shmat(shmid, NULL, 0);
    if (startAddr == (void*)-1) {
        printf("attach shared memory fail\n");
        goto END;
    }
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL);
    if (semid < 0) {
        printf("create semaphore fail\n");
        goto END;
    }
    if (fork() == 0) {
        execl("./read", "./read", NULL);
        exit(0);
    }
    struct sembuf sem_op;
    sem_op.sem_flg = SEM_UNDO;
    //向共享内存写数据 a ab abc abcd .... a~z a ab abc abcd .... a~z 
    const int cnt = 50;
    for (int i = 0;i < cnt;i++) {
        int len = (i + 1) % 26;//0~25
        char buf[27] = { 0 };//0~26
        for (int j = 0;j <= len;j++) {
            buf[j] = j + 'a';
        }
        buf[len + 1] = '\0';
        sem_op.sem_op = -1;//P操作
        semop(semid, &sem_op, 1);//申请信号量
        strcpy(startAddr, buf);
        sem_op.sem_op = 1;//V操作
        semop(semid, &sem_op, 1);//释放信号量
        usleep(1);
    }
    sem_op.sem_op = -1;//P操作
    semop(semid, &sem_op, 1);//申请信号量
    strcpy(startAddr, "quit");
    sem_op.sem_op = 1;//V操作
    semop(semid, &sem_op, 1);//释放信号量
    wait(NULL);
    if (semctl(semid, 0, IPC_RMID) < 0) {
        printf("delete semaphore fail\n");
    }
END:
    shmctl(shmid, IPC_RMID, NULL);//删除共享内存
    return 0;
}