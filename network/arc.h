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

typedef int ArcIndex;

const ArcIndex INVALID_ARC_ID = INT_MAX;
const FlowType MAX_CAPACITY = INT_MAX;

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

    Arc(ArcIndex arc, NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity, bool is_artificial);

    Arc(ArcIndex arc, NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity);

    Arc(ArcIndex arc, NodeIndex src, NodeIndex dst, PriceType cost);

    virtual ~Arc() = default;

    bool IsStatusBasis() const {
        return status_ == FLOW_BASIS;
    }

    bool IsStatusUpper() const {
        return status_ == FLOW_UPPER;
    }

    bool IsStatusLower() const {
        return status_ == FLOW_LOWER;
    }

    void SetStatus(FlowStatus status) {
        status_ = status;
    }
    friend std::ostream &operator<<(std::ostream &os, const Arc &arc);

public:
    ArcIndex arc_id_;
    NodeIndex src_;
    NodeIndex dst_;
    PriceType cost_;
    FlowType capacity_;
    FlowType flow_;
    PriceType reduced_cost_;
    Direction direction_;
    bool is_artificial_;

private:
    FlowStatus status_;

    DISALLOW_COPY_AND_ASSIGN(Arc);
};

}

#endif //NETWORK_SIMPLEX_ARC_H
