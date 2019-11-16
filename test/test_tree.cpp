//
// Created by LyxnO on 2019/11/9.
//

#include <iostream>
#include <string>

#include "tree.h"
#include "network_utils.h"

using namespace std;
using namespace network;

BasisArc MockBasisArc() {
    BasisArc basis_arc{
    {1, 2},
    {3, 0},
    {3, 2},
    {4, 1},
    };
    return basis_arc;
}

int TestInitTree() {
    string filename = "../data/test.nwk";
    TreeAPI tree{};
    ReadNetwork(filename, tree);
    auto basis_arc = MockBasisArc();
    int root = 0;
    tree.InitBasisTree(basis_arc, root);
    printf("Tree\n");
    PrintTree(tree);
    printf("Basis\n");
    PrintBasisArc(tree);
    printf("NonBasis\n");
    PrintNonBasisArc(tree);
    printf("TotalCost=%f", tree.GetTotalCost());
}

int main() {
    TestInitTree();
}