//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_ARC_H
#define NETWORK_SIMPLEX_ARC_H

#include <climits>
#include <ostream>

#include "common.h"
#include "node.h"

namespace network {

const int INVALID_ARC_ID = INT32_MAX;
const int MAX_CAPACITY = INT_MAX;

enum FlowStatus {
    FLOW_LOWER = 0,
    FLOW_UPPER = 1,
    FLOW_BASIS = 2,
};

enum Direction {
    D_POS = 0,
    D_NEG = 1,
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

    bool IsStatusBasis() const;

    bool IsStatusUpper() const;

    bool IsStatusLower() const;

    void SetStatus(FlowStatus status);

    friend std::ostream &operator<<(std::ostream &os, const Arc &arc);

public:
    int arc_id_;
    int src_;
    int dst_;
    double cost_;
    int capacity_;
    int flow_;
    double reduced_cost_;
    FlowStatus status_;
    Direction direction_;
    bool is_artificial_;

private:

    DISALLOW_COPY_AND_ASSIGN(Arc);
};

}

#endif //NETWORK_SIMPLEX_ARC_H
