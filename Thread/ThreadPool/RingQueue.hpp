#pragma once
#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<queue>
#include<vector>
#include<semaphore.h>
using std::vector;
using std::queue;
using std::cout;
using std::endl;
//基于环形队列的生产者消费者模型
template<class TASKPTR>
class RingQueue {
public:
    RingQueue(int cap = 5) :capacity(cap), proIndex(0), conIndex(0), circleQueue(cap, nullptr) {
        sem_init(&blackSem, 0, capacity);
        sem_init(&dataSem, 0, 0);
        pthread_mutex_init(&proMutex, nullptr);
        pthread_mutex_init(&conMutex, nullptr);
    }
    ~RingQueue() {
        sem_destroy(&blackSem);
        sem_destroy(&dataSem);
        pthread_mutex_destroy(&proMutex);
        pthread_mutex_destroy(&conMutex);
    }
    void push_task(TASKPTR task) {
        sem_wait(&blackSem);//P操作
        pthread_mutex_lock(&proMutex);
        circleQueue[proIndex++] = task;
        proIndex %= capacity;
        pthread_mutex_unlock(&proMutex);
        sem_post(&dataSem);//V操作
    }
    TASKPTR pop_task() {
        sem_wait(&dataSem);
        pthread_mutex_lock(&conMutex);
        TASKPTR task = circleQueue[conIndex++];
        conIndex %= capacity;
        pthread_mutex_unlock(&conMutex);
        sem_post(&blackSem);
        return task;
    }
private:
    int proIndex;//生产者位置
    pthread_mutex_t proMutex;//生产者与生产者之间互斥
    int conIndex;//消费者的位置
    pthread_mutex_t conMutex;//消费者与消费者之间互斥
    sem_t blackSem;//标记还有多少空间
    sem_t dataSem;//标记还有多少数据
    vector<TASKPTR> circleQueue;
    int capacity;
};