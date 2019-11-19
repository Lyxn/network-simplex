//
// Created by LyxnO on 2019/11/16.
//

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

    TreeAPI tree{};
    tree.debug_ = debug;
    tree.num_seq_ = 500;
    int ret = ReadNetwork(filename, tree);
    if (ret != RET_SUCCESS) {
        return ret;
    }
    tree.InitArtificialBasis();
    tree.RunSimplex();
    PrintTree(tree);
    PrintArcFlow(tree);
    return 0;
}
