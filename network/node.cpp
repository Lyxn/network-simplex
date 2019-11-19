//
// Created by LyxnO on 2019/11/9.
//

#include "node.h"

namespace network {

Node::Node(int node_id,
           int supply,
           bool is_artificial) : node_id_(node_id),
                                 supply_(supply),
                                 is_artificial_(is_artificial),
                                 is_root_(false),
                                 is_leaf_(false),
                                 father_(INVALID_NODE_ID),
                                 son_(INVALID_NODE_ID),
                                 brother_(INVALID_NODE_ID),
                                 depth_(0),
                                 price_(MAX_PRICE) {}

void Node::SetRoot() {
    is_root_ = true;
    depth_ = 0;
    father_ = INVALID_NODE_ID;
    brother_ = INVALID_NODE_ID;
}

void Node::AddArdDst(int dst) {
    arc_dst_.insert(dst);
}

void Node::AddNeighbor(int node_id) {
    neighbor_.insert(node_id);
}

bool Node::operator==(const Node &rhs) const {
    return node_id_ == rhs.node_id_;
}

bool Node::operator!=(const Node &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const Node &node) {
    os << "node_id: " << node.node_id_
       << " supply: " << node.supply_
       << " depth: " << node.depth_
       << " father: " << node.father_
       << " price: " << node.price_;
    return os;
}

}
