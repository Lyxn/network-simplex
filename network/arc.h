//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_ARC_H
#define NETWORK_SIMPLEX_ARC_H

#include <climits>

#include "common.h"
#include "node.h"

namespace network {

const int INVALID_ARC_ID = -1;
const int MAX_CAPACITY = INT_MAX;

enum FlowStatus {
    FLOW_NONE = 0,
    FLOW_LOWER = 1,
    FLOW_UPPER = 2,
    FLOW_TREE = 3,
};

enum Direction {
    D_NONE = 0,
    D_DOWN = 1,
    D_UP = 2,
};

class Arc {
public:
    Arc() = delete;

    Arc(int arc_id, int src_id, int dst_id, double cost, int capacity, int is_artificial);

    Arc(int arc_id, int src_id, int dst_id, double cost, int capacity);

    Arc(int arc_id, int src_id, int dst_id, double cost);

    virtual ~Arc() = default;

    bool operator==(const Arc &rhs) const;

    bool operator!=(const Arc &rhs) const;

    int GetNeighbor(int node_id) const;

    bool IsStatusTree() const;

    bool IsStatusUpper() const;

    bool IsStatusLower() const;

    void SetStatus(FlowStatus status);

    int GetSrcId() const;

    int GetDstId() const;

public:
    int arc_id_;
    int src_id_;
    int dst_id_;
    double cost_;
    int capacity_;
    int flow_;
    double reduced_cost_;
    FlowStatus status_;
    Direction direction_;

private:
    int _is_artificial_;

    DISALLOW_COPY_AND_ASSIGN(Arc);
};

}

#endif //NETWORK_SIMPLEX_ARC_H
