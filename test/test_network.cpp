//
// Created by LyxnO on 2019/11/9.
//

#include <iostream>
#include <string>

#include "utils.h"
#include "network_utils.h"

using namespace std;
using namespace network;

void PrintPath(const Network &nwk, const vector<int> &path) {
    for (int nid: path) {
        auto p_node = nwk.GetNode(nid);
        auto p_arc = nwk.GetArc(nid, p_node->parent_);
        double cost = (p_arc == nullptr) ? 0 : p_arc->cost_;
        cout << *p_node
             << " cost: " << cost
             << endl;
    }
}

void TestReadNetwork() {
    string filename = "../data/test.nwk";
//    string filename = "../data/petersen.nwk";
    Network nwk{};
    int ret = ReadNetwork(filename, nwk);
    if (ret != RET_SUCCESS) {
        return;
    }
    cout << nwk << endl;
}

void TestShortestPath() {
    string filename = "../data/shortestpath.nwk";
    Network nwk{};
    int ret = ReadNetwork(filename, nwk);
    if (ret != RET_SUCCESS) {
        return;
    }
    printf("Init\n");
    cout << nwk << endl;
    int src = 0;
    int dst = int(nwk.GetNumNodes() - 1);
    auto p_src = nwk.GetNode(src);
    auto p_dst = nwk.GetNode(dst);
    cout << "src\n" << *p_src << endl;
    cout << "dst\n" << *p_dst << endl;
    printf("Bellman\n");
    FindShortestPathBellman(nwk, dst);
    auto path0 = GetPath(nwk, src);
    PrintPath(nwk, path0);
    printf("Dijkstra\n");
    FindShortestPathDijkstra(nwk, dst);
    auto path1 = GetPath(nwk, src);
    PrintPath(nwk, path1);
}

int main() {
    TestReadNetwork();
    TestShortestPath();
}