//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_NETWORK_H
#define NETWORK_SIMPLEX_NETWORK_H

#include <memory>
#include <map>
#include <sstream>
#include <ostream>

#include "hash_type.h"
#include "utils.h"
#include "node.h"
#include "arc.h"

namespace network {

typedef std::shared_ptr<Arc> ArcPtr;
typedef std::shared_ptr<Node> NodePtr;
typedef BiKey<NodeIndex, NodeIndex> ArcKey;
typedef HashBiKey<NodeIndex, NodeIndex> HashArcKey;
typedef std::unordered_map<ArcKey, ArcPtr, HashArcKey> ArcPtrMap;

inline ArcKey GetArcKey(const ArcPtr &p_arc) {
    return {p_arc->src_, p_arc->dst_};
}

class Network {
public:
    Network();

    virtual ~Network() = default;

    friend std::ostream &operator<<(std::ostream &os, const Network &nwk);

    void Clear();

    void AddNode(NodeIndex nid, FlowType supply, bool is_artificial = false);

    NodeIndex AddArc(NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity, bool is_artificial);

    NodeIndex AddArc(NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity);

    NodeIndex AddArtificialArc(NodeIndex src, NodeIndex dst);

    ArcPtr GetArc(ArcIndex aid) const {
        return arcs_.at(aid);
    }

    ArcPtr GetArc(const ArcKey &key) {
        return FindHashMap(arc_idx_, key);
    }

    ArcPtr GetArc(const ArcKey &key) const {
        return FindHashMap(arc_idx_, key);
    }

    ArcPtr GetArc(NodeIndex src, NodeIndex dst) {
        return GetArc({src, dst});
    }

    ArcPtr GetArc(NodeIndex src, NodeIndex dst) const {
        return FindHashMap(arc_idx_, {src, dst});
    }

    bool IsValidNode(NodeIndex nid) const {
        return nid < num_nodes_;
    }

    NodePtr GetNode(NodeIndex nid) const {
        if (!IsValidNode(nid)) {
            return nullptr;
        }
        return nodes_[nid];
    }

    int GetNumNodes() const {
        return num_nodes_;
    }

    int GetNumArcs() const {
        return num_arcs_;
    }

private:
    void AddArcIdx(const ArcPtr &p_arc);

    void ResizeNodes(size_t num);

    std::vector<NodePtr> nodes_;
    std::vector<ArcPtr> arcs_;
    ArcPtrMap arc_idx_;
    PriceType max_cost_;
    int num_nodes_;
    int num_arcs_;

    DISALLOW_COPY_AND_ASSIGN(Network);
};

}
#endif //NETWORK_SIMPLEX_NETWORK_H
