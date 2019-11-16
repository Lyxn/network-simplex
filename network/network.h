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

class Network {
public:
    Network() = default;

    virtual ~Network() = default;

    friend std::ostream &operator<<(std::ostream &os, const Network &nwk);

    void Clear();

    void AddNode(int nid, int supply);

    int AddArc(int src, int dst, double cost, int capacity, bool is_artificial);

    int AddArc(int src, int dst, double cost, int capacity);

    int AddArc(int src, int dst);

    ArcPtr GetArc(int aid) const;

    ArcPtr GetArc(const ArcKey &key);

    ArcPtr GetArc(const ArcKey &key) const;

    ArcPtr GetArc(int src, int dst);

    ArcPtr GetArc(int src, int dst) const;

    NodePtr GetNode(int nid) const;

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
