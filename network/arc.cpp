//
// Created by LyxnO on 2019/11/9.
//

#include "arc.h"

namespace network {

Arc::Arc(int arc_id,
         int src_id,
         int dst_id,
         double cost,
         int capacity,
         int is_artificial) : arc_id_(arc_id),
                              src_id_(src_id),
                              dst_id_(dst_id),
                              cost_(cost),
                              capacity_(capacity),
                              _is_artificial_(is_artificial) {
    flow_ = 0;
    reduced_cost_ = 0.0;
    status_ = FLOW_NONE;
    direction_ = D_NONE;
}

Arc::Arc(int arc_id, int src_id, int dst_id, double cost, int capacity) :
Arc(arc_id, src_id, dst_id, cost, capacity, false) {}

Arc::Arc(int arc_id, int src_id, int dst_id, double cost) :
Arc(arc_id, src_id, dst_id, cost, MAX_CAPACITY, false) {}

bool Arc::operator==(const Arc &rhs) const {
    return src_id_ == rhs.src_id_ &&
           dst_id_ == rhs.dst_id_;
}

bool Arc::operator!=(const Arc &rhs) const {
    return !(rhs == *this);
}

int Arc::GetNeighbor(int node_id) const {
    if (src_id_ == node_id) {
        return dst_id_;
    } else if (dst_id_ == node_id) {
        return src_id_;
    } else {
        return INVALID_NODE_ID;
    }
}

bool Arc::IsStatusTree() const {
    return status_ == FLOW_TREE;
}

bool Arc::IsStatusUpper() const {
    return status_ == FLOW_UPPER;
}

bool Arc::IsStatusLower() const {
    return status_ == FLOW_LOWER;
}
}
