#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<queue>
#include<assert.h>
#include"RingQueue.hpp"
#include"Task.h"
#include"randomNum.h"
#define PRO_NUM 1 //生产者线程数量
#define CON_NUM 2 //消费者线程数量
#define TASK_NUM 20 //每一个生产者生产的任务数量
using std::queue;
using std::cout;
using std::endl;
static RingQueue<Task*> ringQueue;
static const char ops[4] = { '+','-','*','/' };
void* ProductHandler(void* args) {
    int count = TASK_NUM;
    while (count--) {
        // cout << ops[getRandomNumber(0, 3)] << endl;
        Task* task = new Task(getRandomNumber(1, 50), getRandomNumber(1, 50), ops[getRandomNumber(0, 3)]);
        ringQueue.push_task(task);
        // usleep(1000);
        // sleep(1);//生产者生产的慢
    }
    return nullptr;
}
void* ConsumerHandler(void* args) {
    assert((PRO_NUM * TASK_NUM) % CON_NUM == 0);
    int count = (PRO_NUM * TASK_NUM) / CON_NUM;//每一个消费者消费的任务数量
    while (count--) {
        Task* task = ringQueue.pop_task();
        task->operator()();
        delete task;
        // usleep(1000);
        sleep(1);//消费者消费的慢
    }
    return nullptr;
}
int main() {
    pthread_t protids[PRO_NUM];
    pthread_t contids[CON_NUM];
    for (int i = 0;i < PRO_NUM;i++) {
        pthread_create(protids + i, nullptr, ProductHandler, nullptr);
    }
    for (int i = 0;i < CON_NUM;i++) {
        pthread_create(contids + i, nullptr, ConsumerHandler, nullptr);
    }
    for (int i = 0;i < PRO_NUM;i++) {
        pthread_join(protids[i], nullptr);
    }
    for (int i = 0;i < CON_NUM;i++) {
        pthread_join(contids[i], nullptr);
    }
    return 0;
}