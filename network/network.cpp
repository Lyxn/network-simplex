//
// Created by LyxnO on 2019/11/9.
//

#include "network.h"

namespace network {

Network::Network() : max_cost_(0),
                     num_nodes_(0),
                     num_arcs_(0) {}

std::ostream &operator<<(std::ostream &os, const Network &nwk) {
    NodeIndex max_line = 10;
    os << nwk.GetNumNodes() << "\n";

    NodeIndex count = 0;
    for (int i = 0; i < nwk.GetNumNodes(); i++) {
        auto p_node = nwk.GetNode(i);
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
    max_cost_ = 0;
    num_arcs_ = 0;
    num_nodes_ = 0;
}

void Network::AddNode(NodeIndex nid, FlowType supply, bool is_artificial) {
    num_nodes_ += 1;
    ResizeNodes(num_nodes_);
    nodes_[nid] = std::make_shared<Node>(nid, supply, is_artificial);
}

ArcIndex Network::AddArc(NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity, bool is_artificial) {
    NodeIndex aid = GetNumArcs();
    ArcPtr arc(new Arc(aid, src, dst, cost, capacity, is_artificial));
    arcs_.emplace_back(arc);
    nodes_[src]->AddArdDst(dst);
    AddArcIdx(arc);
    if (!is_artificial) {
        max_cost_ += cost;
    }
    num_arcs_ += 1;
    return aid;
}

ArcIndex Network::AddArc(NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity) {
    return AddArc(src, dst, cost, capacity, false);
}

ArcIndex Network::AddArtificialArc(NodeIndex src, NodeIndex dst) {
    return AddArc(src, dst, max_cost_, MAX_CAPACITY, true);
}

void Network::AddArcIdx(const ArcPtr &p_arc) {
    auto key = GetArcKey(p_arc);
    arc_idx_.insert({key, p_arc});
}

void Network::ResizeNodes(size_t num) {
    if (nodes_.size() <= num) {
        nodes_.resize(num + 1);
    }
}

}
