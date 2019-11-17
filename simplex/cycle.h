//
// Created by LyxnO on 2019/11/16.
//

#ifndef NETWORK_SIMPLEX_CYCLE_H
#define NETWORK_SIMPLEX_CYCLE_H

#include<vector>

#include "network.h"

namespace network {

const ArcKey INVALID_ARC_KEY = {INVALID_NODE_ID, INVALID_NODE_ID};

typedef struct cycle_t {
    ArcKey arc_in;
    ArcKey arc_out;
    int node_joint{};
    std::vector<ArcPtr> path_src;
    std::vector<ArcPtr> path_dst;
    bool leaving_from_src;

    cycle_t();

    cycle_t(const ArcKey &arc);

    void Clear();
} Cycle;

}

#endif //NETWORK_SIMPLEX_CYCLE_H
