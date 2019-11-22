//
// Created by LyxnO on 2019/11/16.
//

#include "cycle.h"

namespace network {

Cycle::cycle_t() : arc_in(nullptr),
                   arc_out(nullptr),
                   node_joint(INVALID_NODE_ID) {}

void cycle_t::Clear() {
    arc_in = nullptr;
    arc_out = nullptr;
    node_joint = INVALID_NODE_ID;
    path.clear();
}

}