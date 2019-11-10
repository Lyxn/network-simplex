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
        auto p_arc = nwk.GetArc(nid, p_node->pred_);
        double cost = (p_arc == nullptr) ? 0 : p_arc->cost_;
        cout << *p_node
             << " cost: " << cost
             << endl;
    }
}

int TestReadNetwork() {
    string filename = "../data/test.nwk";
    Network nwk{};
    ReadNetwork(filename, nwk);
    cout << nwk << endl;
}

int TestShortestPath() {
    string filename = "../data/shortestpath.nwk";
    Network nwk{};
    ReadNetwork(filename, nwk);
    printf("Init\n");
    cout << nwk << endl;
    int src = 0;
    int dst = int(nwk.GetNodeNum() - 1);
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
    TestShortestPath();
}