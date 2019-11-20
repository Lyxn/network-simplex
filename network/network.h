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
typedef BiKeyInt ArcKey;
typedef std::unordered_map<ArcKey, ArcPtr, HashBiKeyInt> ArcPtrMap;

inline ArcKey GetArcKey(const ArcPtr &p_arc) {
    return {p_arc->src_, p_arc->dst_};
}

inline ArcKey GetArcKey(int src, int dst) {
    return {src, dst};
}

class Network {
public:
    Network() = default;

    virtual ~Network() = default;

    friend std::ostream &operator<<(std::ostream &os, const Network &nwk);

    void Clear();

    void AddNode(int nid, int supply, bool is_artificial = false);

    int AddArc(int src, int dst, double cost, int capacity, bool is_artificial);

    int AddArc(int src, int dst, double cost, int capacity);

    int AddArtificialArc(int src, int dst);

    ArcPtr GetArc(int aid) const {
        return FindMap(arcs_, aid);
    }

    ArcPtr GetArc(const ArcKey &key) {
        return FindHashMap(arc_idx_, key);
    }

    ArcPtr GetArc(const ArcKey &key) const {
        return FindHashMap(arc_idx_, key);
    }

    ArcPtr GetArc(int src, int dst) {
        return GetArc({src, dst});
    }

    ArcPtr GetArc(int src, int dst) const {
        return FindHashMap(arc_idx_, {src, dst});
    }

    NodePtr GetNode(int nid) const {
        return FindMap(nodes_, nid);
    }

    size_t GetNodeNum() const {
        return nodes_.size();
    }

    size_t GetArcNum() const {
        return arcs_.size();
    }

public:
    std::map<int, NodePtr> nodes_;
    std::map<int, ArcPtr> arcs_;
    ArcPtrMap arc_idx_;
    double max_cost_{};

private:
    int AddArcIdx(const ArcPtr &p_arc);

    DISALLOW_COPY_AND_ASSIGN(Network);
};

}
#endif //NETWORK_SIMPLEX_NETWORK_H
