//
// Created by LyxnO on 2019/11/9.
//

#include "network.h"

namespace network {

std::ostream &operator<<(std::ostream &os, const Network &nwk) {
    int max_line = 10;
    os << nwk.nodes_.size() << "\n";

    int count = 0;
    for (const auto &p_node: nwk.nodes_) {
        os << p_node->node_id_ << ", " << p_node->supply_ << ": ";
        for (auto dst: p_node->arc_dst_) {
            auto p_arc = nwk.GetArc(p_node->node_id_, dst);
            if (p_arc == nullptr) {
                os << dst << ", " << p_arc << "; ";
            } else {
                os << p_arc->dst_ << ", "
                   << p_arc->cost_ << ", ";
                //<< p_arc->capacity_ << "; ";
            }
        }
        os << "\n";
        count += 1;
        if (count >= max_line) {
            break;
        }
    }

    count = 0;
    for (const auto &p_arc: nwk.arcs_) {
        os << p_arc->src_ << ", "
           << p_arc->dst_
           << "\n";
        count += 1;
        if (count > max_line) {
            break;
        }
    }
    return os;
}

void Network::Clear() {
    arcs_.clear();
    nodes_.clear();
    arc_idx_.clear();
    max_cost_ = 0.0;
}

void Network::AddNode(int nid, int supply, bool is_artificial) {
    NodePtr node(new Node(nid, supply, is_artificial));
    nodes_.emplace_back(node);
}

int Network::AddArc(int src, int dst, double cost, int capacity, bool is_artificial) {
    int aid = GetArcNum();
    ArcPtr arc(new Arc(aid, src, dst, cost, capacity, is_artificial));
//    printf("Arc %d src %d dst %d\n", aid, src, dst);
    arcs_.emplace_back(arc);
    nodes_[src]->AddArdDst(dst);
    AddArcIdx(arc);
    if (!is_artificial) {
        max_cost_ += cost;
    }
    return aid;
}

int Network::AddArc(int src, int dst, double cost, int capacity) {
    return AddArc(src, dst, cost, capacity, false);
}

int Network::AddArtificialArc(int src, int dst) {
    return AddArc(src, dst, max_cost_, MAX_CAPACITY, true);
}

int Network::AddArcIdx(const ArcPtr &p_arc) {
    int src = p_arc->src_;
    int dst = p_arc->dst_;
    auto key = GetArcKey(src, dst);
    arc_idx_.insert({key, p_arc});
    return arc_idx_.size();
}


}