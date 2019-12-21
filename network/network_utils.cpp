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

int ParseNodeInfo(const std::string &node_info, NodeIndex &nid, FlowType &supply) {
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

int ParseArcInfo(const std::string &arc_info, NodeIndex &dst, PriceType &cost, FlowType &capacity) {
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
    int num_nodes;
    getline(infile, line);
    ParseStr(line, num_nodes);
    while (getline(infile, line)) {
        vector<string> items;
        SplitStr(line, ":", items);
        if (items.size() != 2) {
            continue;
        }
        //Node
        string node_str = items[0];
        NodeIndex nid;
        FlowType supply;
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
            NodeIndex dst;
            PriceType cost;
            FlowType capacity;
            ParseArcInfo(arc, dst, cost, capacity);
            nwk.AddArc(nid, dst, cost, capacity);
        }
    }
    if (num_nodes != nwk.GetNumNodes()) {
        printf("the node num %d is not the same as the file num %d\n", nwk.GetNumNodes(), num_nodes);
        return RET_ERROR;
    }
    auto last_node = nwk.GetNode(num_nodes - 1);
    if (last_node == nullptr || last_node->node_id_ != num_nodes - 1) {
        printf("last node is nullptr or node_id is not valid\n");
        return RET_ERROR;
    }
    std::cout << "Network Nodes: " << nwk.GetNumNodes()
              << " Arcs: " << nwk.GetNumArcs()
              << std::endl;
    return RET_SUCCESS;
}

int FindShortestPathBellman(Network &nwk, NodeIndex dst) {
    int num_nodes = nwk.GetNumNodes();
    for (int i = 0; i < num_nodes; i++) {
        auto p_node = nwk.GetNode(i);
        p_node->price_ = MAX_PRICE;
    }
    auto p_dst = nwk.GetNode(dst);
    p_dst->price_ = 0;
    p_dst->parent_ = INVALID_NODE_ID;
    do {
        bool has_change = false;
        for (int i = 0; i < nwk.GetNumArcs(); i++) {
            auto p_arc = nwk.GetArc(i);
            if (p_arc->src_ == dst) {
                continue;
            }
            auto p_arc_dst = nwk.GetNode(p_arc->dst_);
            if (p_arc_dst->price_ == MAX_PRICE) {
                continue;
            }
            auto p_arc_src = nwk.GetNode(p_arc->src_);
            auto price = p_arc->cost_ + p_arc_dst->price_;
            if (p_arc_src->price_ > price) {
                p_arc_src->price_ = price;
                p_arc_src->parent_ = p_arc->dst_;
                has_change = true;
            }
        }
        if (!has_change) {
            break;
        }
    } while (true);
    return RET_SUCCESS;
}

int FindShortestPathDijkstra(Network &nwk, NodeIndex dst) {
    int num_nodes = nwk.GetNumNodes();
    for (int i = 0; i < num_nodes; i++) {
        auto p_node = nwk.GetNode(i);
        p_node->price_ = MAX_PRICE;
    }
    auto p_dst = nwk.GetNode(dst);
    p_dst->price_ = 0;
    p_dst->parent_ = INVALID_NODE_ID;
    std::set<NodeIndex> node_shortest;
    do {
        // find min node
        NodeIndex min_node = INVALID_NODE_ID;
        PriceType min_price = MAX_PRICE;
        for (int i = 0; i < num_nodes; i++) {
            auto p_node = nwk.GetNode(i);
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
        for (int i = 0; i < num_nodes; i++) {
            auto p_node = nwk.GetNode(i);
            if (HasValue(node_shortest, p_node->node_id_)) {
                continue;
            }
            for (auto arc_dst: p_node->arc_dst_) {
                auto p_arc = nwk.GetArc(p_node->node_id_, arc_dst);
                auto p_arc_dst = nwk.GetNode(arc_dst);
                if (p_arc_dst->price_ == MAX_PRICE) {
                    continue;
                }
                PriceType price = p_arc->cost_ + p_arc_dst->price_;
                if (p_node->price_ > price) {
                    p_node->parent_ = arc_dst;
                    p_node->price_ = price;
                }
            }
        }
    } while (true);

    return RET_SUCCESS;
}

std::vector<NodeIndex> GetPath(const Network &nwk, NodeIndex src) {
    std::vector<NodeIndex> path;
    auto next = src;
    while (next != INVALID_NODE_ID) {
        path.push_back(next);
        auto p_src = nwk.GetNode(next);
        if (p_src == nullptr) {
            printf("ERROR NodeID=%d", next);
            break;
        }
        next = p_src->parent_;
    }
    return path;
}

}