#pragma once
#include<random>
using std::mt19937;
using std::random_device;
inline static int getRandomNumber(int L, int R) {
    //产生[L,R]之间的随机数
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<> dis(L, R);
    return dis(gen);
}