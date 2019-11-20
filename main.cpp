//
// Created by LyxnO on 2019/11/16.
//

#include <ctime>
#include <iostream>

#include "tree.h"
#include "network_utils.h"

using namespace std;
using namespace network;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        auto self = argv[0];
        printf("USAGE:\n%s $FILE_NWK [$DEBUG]\n", self);
        return 0;
    }
    auto filename = string(argv[1]);
    bool debug = argc >= 3;

    clock_t start;
    TreeAPI tree{};
    tree.debug_ = debug;

    start = clock();
    int ret = ReadNetwork(filename, tree);
    printf("Network TimeCost %ld\n", clock() - start);
    if (ret != RET_SUCCESS) {
        return ret;
    }

    start = clock();
    tree.InitArtificialBasis();
    printf("Basis TimeCost %ld\n", clock() - start);

    start = clock();
    tree.RunSimplex();
    printf("Simplex TimeCost %ld\n", clock() - start);

    PrintTree(tree);
    PrintArcFlow(tree);
    return 0;
}
