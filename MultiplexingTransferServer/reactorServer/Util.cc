#include "Util.h"
vector<string> SplitString(string& message, string sep) {
    //message1/Xmessage2/Xmessage3...
    vector<string> ans;
    int start = 0;
    int pos = 0;
    while ((pos = message.find(sep, start)) != string::npos) {
        //start=0,pos=3
        ans.push_back(message.substr(start, pos - start));
        start = pos + sep.size();
    }
    message = message.substr(start);
    return ans;
}
mathcalc::Response ConstructResponse(string& requestStr) {
    mathcalc::Response response;
    mathcalc::Request request;
    request.ParseFromString(requestStr);
    string op = request.op();
    int lnum = request.leftnum();
    int rnum = request.rightnum();
    response.set_leftnum(lnum);
    response.set_rightnum(rnum);
    response.set_op(request.op());
    if (op.size() != 1) {
        response.set_ans(0);
        response.set_flag(false);
        return response;
    }
    response.set_flag(true);
    switch (op[0])
    {
    case '+':
        response.set_ans(lnum + rnum);
        break;
    case '-':
        response.set_ans(lnum - rnum);
        break;
    case '*':
        response.set_ans(lnum * rnum);
        break;
    case '/':
        if (rnum == 0) {
            response.set_flag(false);
            response.set_ans(0);
        }
        else {
            response.set_ans(lnum / rnum);
        }
        break;
    default:
        response.set_flag(false);
        response.set_ans(0);
        break;
    }
    return response;
}
bool SetNoBlock(int sock) {
    int fl = fcntl(sock, F_GETFL);
    if (fl < 0) {
        return false;
    }
    fl |= O_NONBLOCK;
    return fcntl(sock, F_SETFL, fl) == 0;
}