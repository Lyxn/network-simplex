//
// Created by LyxnO on 2019/11/10.
//

#ifndef NETWORK_SIMPLEX_HASH_TYPE_H
#define NETWORK_SIMPLEX_HASH_TYPE_H

#include <functional>
#include <unordered_map>

namespace network {

template<class T1, class T2>
struct BiKey {
    T1 first;
    T2 second;

    BiKey(T1 f, T2 s) : first(f), second(s) {}
};

template<class T1, class T2>
struct HashBiKey {
    std::size_t operator()(const BiKey<T1, T2> &key) const {
        auto h1 = std::hash<T1>()(key.first);
        auto h2 = std::hash<T2>()(key.second);
        return h1 ^ (h2 << 1);
    }
};

template<class T1, class T2>
struct EqualBiKey {
    bool operator()(const BiKey<T1, T2> &lhs, const BiKey<T1, T2> &rhs) const {
        return lhs.first == rhs.first
               && lhs.second == rhs.second;
    }
};

template<class T1, class T2, class HASH, class EQ>
T2 FindHashMap(const std::unordered_map<T1, T2, HASH, EQ> &items, const T1 &key) {
    auto it = items.find(key);
    if (it != items.end()) {
        return it->second;
    }
    return T2();
}

template<class T1, class T2, class HASH, class EQ>
T2 HasHashKey(const std::unordered_map<T1, T2, HASH, EQ> &items, const T1 &key) {
    auto it = items.find(key);
    if (it != items.end()) {
        return it->second;
    }
    return T2();
}

typedef BiKey<int, int> BiKeyInt;
typedef HashBiKey<int, int> HashBiKeyInt;
typedef EqualBiKey<int, int> EqualBiKeyInt;

}

#endif //NETWORK_SIMPLEX_HASH_TYPE_H
