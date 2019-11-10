//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_UTILS_H
#define NETWORK_SIMPLEX_UTILS_H

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace network {

template<class T1>
bool HasValue(const std::set<T1> &items, const T1 &val) {
    return items.find(val) != items.end();
}

template<class T1, class T2>
bool HasKey(const std::map<T1, T2> &items, const T1 &key) {
    return items.find(key) != items.end();
}

template<class T1, class T2>
T2 FindMap(const std::map<T1, T2> &items, const T1 &key) {
    auto it = items.find(key);
    if (it != items.end()) {
        return it->second;
    }
    return T2();
}

template<class T>
std::string ToStr(const T &d) {
    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10);
    ss << d;
    return ss.str();
}

template<class T>
bool ParseStr(const std::string &str, T &res) {
    T trans;
    std::stringstream ss;
    ss << str;
    ss >> trans;
    if (!ss.fail()) {
        res = trans;
    }
    return !ss.fail();
}

void SplitStr(const std::string &str, const std::string &sep, std::vector<std::string> &items);

}

#endif //NETWORK_SIMPLEX_UTILS_H
