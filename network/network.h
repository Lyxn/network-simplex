//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_NETWORK_H
#define NETWORK_SIMPLEX_NETWORK_H

#include <memory>
#include <map>
#include <string>
#include <sstream>
#include <ostream>

#include "hash_type.h"
#include "node.h"
#include "arc.h"

namespace network {

using std::map;
using std::string;

typedef std::shared_ptr<Arc> ArcPtr;
typedef std::shared_ptr<Node> NodePtr;

typedef std::unordered_map<BiKeyInt, ArcPtr, HashBiKeyInt, EqualBiKeyInt> ArcPtrMap;

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
    map<int, NodePtr> nodes_;
    map<int, ArcPtr> arcs_;
    ArcPtrMap arc_idx_;
    double max_cost_{};

private:
    int AddArcIdx(int src, int dst, const ArcPtr &p_arc);

    DISALLOW_COPY_AND_ASSIGN(Network);
};

}
#endif //NETWORK_SIMPLEX_NETWORK_H
