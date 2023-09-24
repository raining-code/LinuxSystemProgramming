#include <string>
#include <iostream>
using namespace std;
int main() {
    string s = "abcd";
    s = s.substr(3);
    cout << s << endl;
    return 0;
}