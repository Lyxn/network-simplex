#include <iostream>

#include "tree.h"
#include "network_utils.h"

using namespace std;
using namespace network;

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        auto self = argv[0];
        printf("USAGE:\n%s $FILE_NWK [$DEBUG]\n", self);
        return 0;
    }
    auto filename = string(argv[1]);
    bool debug = argc >= 3;

    TreeAPI tree{};
    tree.debug_ = debug;
    ReadNetwork(filename, tree);
    tree.InitArtificialBasis();
    tree.RunSimplex();
    return 0;
}
