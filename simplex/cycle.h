//
// Created by LyxnO on 2019/11/16.
//

#ifndef NETWORK_SIMPLEX_CYCLE_H
#define NETWORK_SIMPLEX_CYCLE_H

#include<vector>

#include "network.h"

namespace network {

typedef struct cycle_t {
    ArcPtr arc_in;
    ArcPtr arc_out;
    int node_joint;
    std::vector<ArcPtr> path;

    cycle_t();

    void Clear();
} Cycle;

}

#endif //NETWORK_SIMPLEX_CYCLE_H
