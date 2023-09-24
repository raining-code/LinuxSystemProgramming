#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdbool.h>
//使用互斥锁+条件变量实现线程同步
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void* DisplayMessage(void* Pnum) {
    int num = (int)Pnum;
    int count = 5;
    while (count--) {
        pthread_cond_wait(&cond, &mutex);
        printf("I am thread %d\n", num);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main() {
    pthread_t tids[5];
    for (int i = 0;i < 5;i++) {
        pthread_create(tids + i, NULL, DisplayMessage, (void*)i);
    }
#ifdef SIGNAL
    int total = 5 * 5;
#endif
#ifdef BROADCAST
    int total = 5;
#endif
    while (total > 0) {
        getchar();
#ifdef SIGNAL
        pthread_cond_signal(&cond);
#endif
#ifdef BROADCAST
        pthread_cond_broadcast(&cond);
#endif
        total--;
    }
    usleep(5000);
    printf("%d\n", total);
    for (int i = 0;i < 5;i++) {
        pthread_join(tids[i], NULL);
    }
    return 0;
}