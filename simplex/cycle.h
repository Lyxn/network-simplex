//
// Created by LyxnO on 2019/11/16.
//

#ifndef NETWORK_SIMPLEX_CYCLE_H
#define NETWORK_SIMPLEX_CYCLE_H

#include<vector>

#include "network.h"

namespace network {

class Cycle {
public:
    int arc_in_;
    int arc_out_;
    int node_join_;
};

}

#endif //NETWORK_SIMPLEX_CYCLE_H
