#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "mathcalc.pb.h"
#include <fcntl.h>
using std::string;
using std::cout;
using std::endl;
using std::vector;
vector<string> SplitString(string& message, string sep);
mathcalc::Response ConstructResponse(string& request);
//设置一个文件描述符为非阻塞
bool SetNoBlock(int sock);