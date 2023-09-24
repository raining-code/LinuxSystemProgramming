#pragma once
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iostream>
#include<fcntl.h>
#include<signal.h>
using std::cout;
using std::endl;
void DaemonOriginal();//原生创建守护进程
void MyDaemon(int dir, int close);//实现系统调用daemon
