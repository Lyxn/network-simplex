//
// Created by LyxnO on 2019/11/9.
//

#include "network_utils.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "utils.h"

namespace network {

using std::string;
using std::vector;

int ParseNodeInfo(const std::string &node_info, int &nid, int &supply) {
    vector<string> items;
    SplitStr(node_info, ",", items);
    ParseStr(items[0], nid);
    if (items.size() < 2) {
        supply = 0;
    } else {
        ParseStr(items[1], supply);
    }
    return RET_SUCCESS;
}

int ParseArcInfo(const std::string &arc_info, int &dst, double &cost, int &capacity) {
    vector<string> items;
    SplitStr(arc_info, ",", items);
    ParseStr(items[0], dst);
    ParseStr(items[1], cost);
    if (items.size() < 3) {
        capacity = MAX_CAPACITY;
    } else {
        ParseStr(items[2], capacity);
    }
    return RET_SUCCESS;
}

int ReadNetwork(const std::string &filename, Network &nwk) {
    std::ifstream infile(filename.c_str());
    if (!infile) {
        printf("Fail to Read File %s\n", filename.c_str());
        return RET_FAIL;
    }
    string line;
    int num_node;
    getline(infile, line);
    ParseStr(line, num_node);
    while (getline(infile, line)) {
        vector<string> items;
        SplitStr(line, ":", items);
        if (items.size() != 2) {
            continue;
        }
        //Node
        string node_str = items[0];
        int nid;
        int supply;
        ParseNodeInfo(node_str, nid, supply);
        nwk.AddNode(nid, supply);
        //Arcs
        string arc_str = items[1];
        if (arc_str.empty()) {
            continue;
        }
        vector<string> arc_vec;
        SplitStr(arc_str, ";", arc_vec);
        for (auto &arc: arc_vec) {
            int dst;
            double cost;
            int capacity;
            ParseArcInfo(arc, dst, cost, capacity);
            nwk.AddArc(nid, dst, cost, capacity);
        }
    }
    if (num_node != nwk.GetNodeNum()) {
        printf("the node num %d is not the same as the file num %d", nwk.GetNodeNum(), num_node);
        return RET_ERROR;
    }
    std::cout << "Network Nodes: " << nwk.GetNodeNum()
              << " Arcs: " << nwk.GetArcNum()
              << std::endl;
    return RET_SUCCESS;
}

int FindShortestPathBellman(Network &nwk, int dst) {
    for (auto &p_node: nwk.nodes_) {
        p_node->price_ = MAX_PRICE;
    }
    auto p_dst = nwk.GetNode(dst);
    p_dst->price_ = 0;
    p_dst->father_ = INVALID_NODE_ID;
    do {
        bool has_change = false;
        for (auto &p_arc: nwk.arcs_) {
            if (p_arc->src_ == dst) {
                continue;
            }
            auto p_arc_dst = nwk.GetNode(p_arc->dst_);
            if (p_arc_dst->price_ == MAX_PRICE) {
                continue;
            }
            auto p_arc_src = nwk.GetNode(p_arc->src_);
            double price = p_arc->cost_ + p_arc_dst->price_;
            if (p_arc_src->price_ > price) {
                p_arc_src->price_ = price;
                p_arc_src->father_ = p_arc->dst_;
                has_change = true;
            }
        }
        if (!has_change) {
            break;
        }
    } while (true);
    return RET_SUCCESS;
}

int FindShortestPathDijkstra(Network &nwk, int dst) {
    for (auto &p_node: nwk.nodes_) {
        p_node->price_ = MAX_PRICE;
    }
    auto p_dst = nwk.GetNode(dst);
    p_dst->price_ = 0;
    p_dst->father_ = INVALID_NODE_ID;
    std::set<int> node_shortest;
    do {
        // find min node
        int min_node = INVALID_NODE_ID;
        double min_price = MAX_PRICE;
        for (auto &p_node: nwk.nodes_) {
            if (HasValue(node_shortest, p_node->node_id_)) {
                continue;
            }
            if (p_node->price_ < min_price) {
                min_node = p_node->node_id_;
                min_price = p_node->price_;
            }
        }
        // no node need to be updated
        if (min_node == INVALID_NODE_ID) {
            break;
        }
        node_shortest.insert(min_node);
        // update node price
        for (auto &p_node: nwk.nodes_) {
            if (HasValue(node_shortest, p_node->node_id_)) {
                continue;
            }
            for (auto arc_dst: p_node->arc_dst_) {
                auto p_arc = nwk.GetArc(p_node->node_id_, arc_dst);
                auto p_arc_dst = nwk.GetNode(arc_dst);
                if (p_dst->price_ == MAX_PRICE) {
                    continue;
                }
                double price = p_arc->cost_ + p_arc_dst->price_;
                if (p_node->price_ > price) {
                    p_node->father_ = arc_dst;
                    p_node->price_ = price;
                }
            }
        }
    } while (true);

    return RET_SUCCESS;
}

std::vector<int> GetPath(const Network &nwk, int src) {
    std::vector<int> path;
    auto next = src;
    while (next != INVALID_NODE_ID) {
        path.push_back(next);
        auto p_src = nwk.GetNode(next);
        if (p_src == nullptr) {
            printf("ERROR NodeID=%d", next);
            break;
        }
        next = p_src->father_;
    }
    return path;
}

}