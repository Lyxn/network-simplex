//
// Created by LyxnO on 2019/11/9.
//

#include <string>

#include "tree.h"
#include "network_utils.h"

using namespace std;
using namespace network;

BasisArcKey MockBasis0() {
    BasisArcKey basis_arc{
    {1, 2},
    {3, 0},
    {3, 2},
    {4, 1},
    };
    return basis_arc;
}

void TestTree0() {
    string filename = "../data/test.nwk";
    TreeAPI tree{};
    ReadNetwork(filename, tree);
    auto basis_arc = MockBasis0();
    int root = 0;
    tree.InitBasisTree(basis_arc, root);
//    tree.InitArtificialBasis();
    printf("Tree\n");
    PrintTree(tree);
    printf("Basis\n");
    PrintBasisArc(tree);
    printf("NonBasis\n");
    PrintNonBasisArc(tree);
}

void TestSimplex0() {
    string filename = "../data/test.nwk";
    int root = 3;
    bool debug = true;
    TreeAPI tree(root, debug);
    ReadNetwork(filename, tree);
    auto basis_arc = MockBasis0();
    tree.InitBasisTree(basis_arc, root);
    printf("Before\n");
    PrintTreeAndBasis(tree);
    tree.num_seq_ = 4;
    tree.RunSimplex();
}

void TestSimplex1() {
    string filename = "../data/beispiel.nwk";
    int root = 0;
    bool debug = true;
    TreeAPI tree(root, debug);
    ReadNetwork(filename, tree);
    tree.InitArtificialBasis();
    tree.num_seq_ = 4;
    tree.RunSimplex(9);
    PrintTreeAndBasis(tree);
}

int main() {
    TestSimplex1();
}