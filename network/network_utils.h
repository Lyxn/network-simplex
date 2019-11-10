//
// Created by LyxnO on 2019/11/9.
//

#ifndef NETWORK_SIMPLEX_NETWORK_UTILS_H
#define NETWORK_SIMPLEX_NETWORK_UTILS_H

#include <vector>

#include "network.h"

namespace network {

int ReadNetwork(const std::string &file_nwk, Network &nwk);

int FindShortestPathBellman(Network &nwk, int dst);

int FindShortestPathDijkstra(Network &nwk, int dst);

std::vector<int> GetPath(const Network &nwk, int dst);

}

#endif //NETWORK_SIMPLEX_NETWORK_UTILS_H
