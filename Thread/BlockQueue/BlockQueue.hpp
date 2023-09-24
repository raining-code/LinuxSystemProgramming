#pragma once
#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<queue>
using std::queue;
using std::cout;
using std::endl;
template<typename TASKPTR>
class BlockQueue {
public:
    BlockQueue(int StartCap = 5) :capacity(StartCap) {//设置起始容量
        pthread_mutex_init(&QueueMutex, nullptr);
        pthread_cond_init(&fullCond, nullptr);
        pthread_cond_init(&emptyCond, nullptr);
    }
    void push_task(TASKPTR task) {//生产者生产任务,调用push_task函数
        pthread_mutex_lock(&QueueMutex);
        while (Queue.size() == capacity) {
            pthread_cond_wait(&fullCond, &QueueMutex);
        }
        Queue.push(task);
        pthread_mutex_unlock(&QueueMutex);
        pthread_cond_signal(&emptyCond);
    }
    TASKPTR pop_task() {//消费者消费任务,调用pop_task函数
        pthread_mutex_lock(&QueueMutex);
        while (Queue.empty()) {
            pthread_cond_wait(&emptyCond, &QueueMutex);
        }
        TASKPTR task = Queue.front();
        Queue.pop();
        pthread_mutex_unlock(&QueueMutex);
        pthread_cond_signal(&fullCond);
        return task;
    }
    ~BlockQueue() {
        pthread_mutex_destroy(&QueueMutex);
        pthread_cond_destroy(&emptyCond);
        pthread_cond_destroy(&fullCond);
    }
private:
    pthread_mutex_t QueueMutex;//保护阻塞队列
    pthread_cond_t emptyCond;//是否为空
    pthread_cond_t fullCond;//是否为满
    int capacity = 0;//阻塞队列的容量
    queue<TASKPTR> Queue;//Queue中存放new出来的任务
};