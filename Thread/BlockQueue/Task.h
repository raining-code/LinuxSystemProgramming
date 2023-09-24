#pragma once
#include<iostream>
using std::cout;
using std::endl;
class Task {
public:
    Task(int l = 0, int r = 0, char o = '+') :left_op(l), right_op(r), op(o) {}
    void operator()()const {
        int ret;
        switch (op) {
        case '+':
            ret = left_op + right_op;
            break;
        case '-':
            ret = left_op - right_op;
            break;
        case '*':
            ret = left_op * right_op;
            break;
        case '/':
            if (right_op) {
                ret = left_op / right_op;
            }
            else {
                ret = INT32_MAX;
            }
            break;
        default:
            ret = INT32_MIN;
            break;
        }
        cout << left_op << ' ' << op << ' ' << right_op << " = " << ret << endl;
    }
private:
    int left_op = 0;
    int right_op = 0;
    char op = '+';
};