//
// Created by LyxnO on 2019/11/9.
//

#include <string>

#include "tree.h"
#include "network_utils.h"

using namespace std;
using namespace network;

BasisArc MockBasis0() {
    BasisArc basis_arc{
    {1, 2},
    {3, 0},
    {3, 2},
    {4, 1},
    };
    return basis_arc;
}

int TestTree0() {
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
    printf("TotalCost=%f\n", tree.GetTotalCost());
}

int TestSimplex0() {
    string filename = "../data/test.nwk";
    TreeAPI tree{};
    ReadNetwork(filename, tree);
    auto basis_arc = MockBasis0();
    int root = 3;
    tree.InitBasisTree(basis_arc, root);
    printf("Before\n");
    PrintTreeAndBasis(tree);
    tree.debug_ = true;
    tree.RunSimplex();
}

int TestSimplex1() {
    string filename = "../data/beispiel.nwk";
    TreeAPI tree{};
    ReadNetwork(filename, tree);
    tree.InitArtificialBasis();
    tree.debug_ = true;
    tree.RunSimplex(9);
    PrintTreeAndBasis(tree);
}

int main() {
    TestSimplex1();
}