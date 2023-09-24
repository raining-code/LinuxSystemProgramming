#include"ThreadPool.hpp"
#include"Task.h"
using std::cin;
// #include"randomNum.h"
#define PRO_NUM 1 //生产者线程数量
// #define TASK_NUM 500 //每一个生产者生产的任务数量
// static const char ops[4] = { '+','-','*','/' };
Task* MakeTask() {
    int l_op = 0;
    int r_op = 0;
    char op = 0;
BEGIN:
    cout << "please input left number:\n";
    cin >> l_op;
    cin.ignore(256, '\n');
    cout << "please input right number:\n";
    cin >> r_op;
    cin.ignore(256, '\n');
    cout << "please input operator(\"+ - * /\"):\n";
    cin >> op;
    cin.ignore(256, '\n');
    if (op != '+' && op != '-' && op != '*' && op != '/') {
        cout << "input error" << endl;
        goto BEGIN;
    }
    return new Task(l_op, r_op, op);
}
void* ProductHandler(void* args) {
    ThreadPool<Task*>* PtrThreadPool = reinterpret_cast<ThreadPool<Task*>*>(args);
    // int count = TASK_NUM;
    while (/*count--*/1) {
        // Task* task = new Task(getRandomNumber(1, 50), getRandomNumber(1, 50), ops[getRandomNumber(0, 3)]);
        Task* task = MakeTask();
        PtrThreadPool->push_task(task);
        // usleep(1000);
        // sleep(1);//生产者生产的慢
    }
    return nullptr;
}
int main() {
    ThreadPool<Task*> threadPool(50, 100);
    pthread_t protids[PRO_NUM];
    for (int i = 0;i < PRO_NUM;i++) {
        pthread_create(protids + i, nullptr, ProductHandler, &threadPool);
    }
    for (int i = 0;i < PRO_NUM;i++) {
        pthread_join(protids[i], nullptr);
    }
    return 0;
}