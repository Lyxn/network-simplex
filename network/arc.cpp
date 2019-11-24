//
// Created by LyxnO on 2019/11/9.
//

#include "arc.h"

namespace network {

Arc::Arc(ArcIndex arc_id,
         NodeIndex src,
         NodeIndex dst,
         PriceType cost,
         FlowType capacity,
         bool is_artificial) : arc_id_(arc_id),
                               src_(src),
                               dst_(dst),
                               cost_(cost),
                               capacity_(capacity),
                               is_artificial_(is_artificial) {
    flow_ = 0;
    status_ = FLOW_LOWER;
    direction_ = D_POS;
    reduced_cost_ = 0;
}

Arc::Arc(ArcIndex arc_id, NodeIndex src, NodeIndex dst, PriceType cost, FlowType capacity) :
Arc(arc_id, src, dst, cost, capacity, false) {}

Arc::Arc(ArcIndex arc_id, NodeIndex src, NodeIndex dst, PriceType cost) :
Arc(arc_id, src, dst, cost, MAX_CAPACITY, false) {}


std::ostream &operator<<(std::ostream &os, const Arc &arc) {
    os << "src: " << arc.src_ << " dst: " << arc.dst_ << " cost: " << arc.cost_ << " cap: "
       << arc.capacity_ << " flow: " << arc.flow_ << " dir: " << arc.direction_;
    return os;
}

}
