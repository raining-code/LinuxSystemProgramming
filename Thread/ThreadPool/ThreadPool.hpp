#pragma once
#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<queue>
#include<vector>
#include<functional>
#include<semaphore.h>
#include"RingQueue.hpp"
using std::vector;
using std::queue;
using std::cout;
using std::endl;
template<typename TASKPTR>
class ThreadPool {//基于环形队列实现线程池
public:
    ThreadPool(int num = 5, int capacity = 20/*环形队列的大小*/) :threadNum(num), threads(num), ringQueue(capacity) {
        for (int i = 0;i < threadNum;i++) {
            pthread_create(threads.data() + i, nullptr, ExecuteTask, this);
        }
    }
    ~ThreadPool() {
        for (int i = 0;i < threadNum;i++) {
            pthread_join(threads[i], nullptr);
        }
    }
    void push_task(TASKPTR task) {
        ringQueue.push_task(task);
    }
private:
    static void* ExecuteTask(void* args) {
        ThreadPool<TASKPTR>* thisPtr = reinterpret_cast<ThreadPool<TASKPTR>*>(args);
        while (1) {
            TASKPTR task = thisPtr->ringQueue.pop_task();
            (*task)();
            delete task;
        }
    }
private:
    RingQueue<TASKPTR> ringQueue;
    vector<pthread_t> threads;//保存所有线程的线程id
    const int threadNum;//线程池中线程的数量
};