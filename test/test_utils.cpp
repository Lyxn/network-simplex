//
// Created by LyxnO on 2019/11/9.
//

#include <iostream>

#include "utils.h"

using namespace std;
using namespace network;

int main() {
    int num = 10;
    string str = ToStr(num);
    cout << str << endl;
    ParseStr(" 11 ", num);
    cout << num << endl;
}