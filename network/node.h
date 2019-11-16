//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_NODE_H
#define NETWORK_SIMPLEX_NODE_H

#include <cfloat>

#include <set>
#include <ostream>

#include "common.h"

namespace network {

const int INVALID_NODE_ID = -1;
const double MAX_PRICE = DBL_MAX;

class Node {
public:
    Node() = delete;

    Node(int node_id, int supply);

    virtual ~Node() = default;

    bool operator==(const Node &rhs) const;

    bool operator!=(const Node &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const Node &node);

    void set_root();

    void AddArdDst(int arc_id);

    void AddNeighbor(int node_id);

public:
    int node_id_;
    int supply_;
    std::set<int> arc_dst_;
    std::set<int> neighbor_;

    //Spanning Tree
    int is_root_;
    int is_leaf_;
    int depth_;
    int pred_;

    //XPI
    int son_;
    int brother_;
    int num_succ_;

    //Price
    double price_;

private:
    DISALLOW_COPY_AND_ASSIGN(Node);
};

}

#endif //NETWORK_SIMPLEX_NODE_H
