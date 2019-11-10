//
// Created by LyxnO on 2019/11/9.
//

#include "network.h"

#include "common/utils.h"

namespace network {

inline BiKeyInt GetArcKey(int src, int dst) {
    return {src, dst};
}

std::ostream &operator<<(std::ostream &os, const Network &nwk) {
    int max_line = 10;
    os << nwk.nodes_.size() << "\n";

    int count = 0;
    for (const auto &it: nwk.nodes_) {
        auto nid = it.first;
        auto p_node = it.second;
        os << nid << ", " << p_node->supply_ << ": ";
        for (auto dst: p_node->arc_dst_) {
            auto p_arc = nwk.GetArc(nid, dst);
            if (p_arc == nullptr) {
                os << dst << ", " << p_arc << "; ";
            } else {
                os << p_arc->dst_id_ << ", "
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
    for (const auto &it: nwk.arcs_) {
        auto p_arc = it.second;
        os << it.first << ": "
           << p_arc->src_id_ << ", "
           << p_arc->dst_id_
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

void Network::AddNode(int nid, int supply) {
    NodePtr node(new Node(nid, supply));
    nodes_.insert({nid, node});
}

int Network::AddArc(int src, int dst, double cost, int capacity, bool is_artificial) {
    max_cost_ += cost;
    int aid = GetArcNum();
    ArcPtr arc(new Arc(aid, src, dst, cost, capacity, is_artificial));
//    printf("Arc %d src %d dst %d\n", aid, src, dst);
    arcs_.insert({aid, arc});
    nodes_[src]->AddArdDst(dst);
    AddArcIdx(src, dst, arc);
    return aid;
}

int Network::AddArc(int src, int dst, double cost, int capacity) {
    return AddArc(src, dst, cost, capacity, false);
}

int Network::AddArc(int src, int dst) {
    return AddArc(src, dst, 0, MAX_CAPACITY, true);
}

int Network::AddArcIdx(int src, int dst, const ArcPtr &p_arc) {
    auto key = GetArcKey(src, dst);
    arc_idx_.insert({key, p_arc});
    return arc_idx_.size();
}

ArcPtr Network::GetArc(int aid) const {
    return FindMap(arcs_, aid);
}

ArcPtr Network::GetArc(int src, int dst) {
    auto key = GetArcKey(src, dst);
    return FindHashMap(arc_idx_, key);
}

ArcPtr Network::GetArc(int src, int dst) const {
    auto key = GetArcKey(src, dst);
    return FindHashMap(arc_idx_, key);
}

NodePtr Network::GetNode(int nid) const {
    return FindMap(nodes_, nid);
}

}