//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_NODE_H
#define NETWORK_SIMPLEX_NODE_H

#include <climits>

#include <set>
#include <ostream>

#include "common.h"

namespace network {

typedef int NodeIndex;
typedef long PriceType;
typedef int FlowType;

const NodeIndex INVALID_NODE_ID = INT_MAX;
const PriceType MAX_PRICE = LONG_MAX;

class Node {
public:
    Node() = delete;

    Node(NodeIndex node_id, FlowType supply, bool is_artificial = false);

    virtual ~Node() = default;

    bool operator==(const Node &rhs) const;

    bool operator!=(const Node &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const Node &node);

    void SetRoot();

    void AddArdDst(int arc_id);

    void AddNeighbor(int node_id);

public:
    NodeIndex node_id_;
    FlowType supply_;
    bool is_artificial_;
    std::set<NodeIndex> arc_dst_;
    std::set<NodeIndex> neighbor_;

    //Spanning Tree
    bool is_root_;
    NodeIndex father_;
    NodeIndex son_;
    NodeIndex brother_;
    int depth_;

    //Price
    PriceType price_;

private:
    DISALLOW_COPY_AND_ASSIGN(Node);
};

}

#endif //NETWORK_SIMPLEX_NODE_H
