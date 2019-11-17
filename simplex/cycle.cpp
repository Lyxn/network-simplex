//
// Created by LyxnO on 2019/11/16.
//

#include "cycle.h"

namespace network {

Cycle::cycle_t() : arc_in(INVALID_ARC_KEY),
                   arc_out{INVALID_ARC_KEY},
                   node_joint(INVALID_NODE_ID) {}

void cycle_t::Clear() {
    arc_in = INVALID_ARC_KEY;
    arc_out = INVALID_ARC_KEY;
    node_joint = INVALID_NODE_ID;
    path_src.clear();
    path_dst.clear();
}

}