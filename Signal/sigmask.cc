#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
using std::cout;
using std::endl;
void DisplayPending(const sigset_t* sigset) {
    for (int i = 1;i < 32;i++) {
        cout << sigismember(sigset, i) << ' ';
    }
    cout << endl;
}
int main() {
    signal(SIGINT, [](int signum) {cout << "catch " << signum << " signal" << endl;});
    sigset_t mySigset;
    sigemptyset(&mySigset);
    sigaddset(&mySigset, SIGINT);
    sigprocmask(SIG_SETMASK, &mySigset, nullptr);//屏蔽2号信号
    int count = 0;
    while (1) {
        sigset_t pendSet;
        sigpending(&pendSet);//获取未决信号集
        DisplayPending(&pendSet);
        sleep(1);
        count++;
        if (count == 10) {//解除阻塞
            sigdelset(&mySigset, SIGINT);
            sigprocmask(SIG_SETMASK, &mySigset, nullptr);
            cout << "解除对2号信号的阻塞" << endl;
        }
    }
    return 0;
}