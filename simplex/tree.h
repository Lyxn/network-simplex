//
// Created by LyxnO on 2019/11/16.
//

#ifndef NETWORK_SIMPLEX_TREE_H
#define NETWORK_SIMPLEX_TREE_H

#include <set>
#include <unordered_set>
#include <vector>

#include "network.h"

namespace network {

typedef std::set<ArcKey> BasisArc;

class TreeAPI : public Network {
public:
    TreeAPI() = default;

    ~TreeAPI() override = default;

    int InitBasisTree(const BasisArc &basis_arc, int root = 0);

    double GetTotalCost() const;

    bool IsBasisArc(const ArcKey &key) const;

    int root_{};
    BasisArc basis_arc_;

private:
    int SetRoot(int root);

    int InitTreeStruct(int root);

    void UpdateDepth(int nid);

    ArcPtr GetBasisArc(int src, int dst);

    void CalcBasisFlow(const BasisArc &basis_arc);

    void CalcNodePrice(NodePtr cur, NodePtr unk);

    void CalcBasisPrice();

    void CalcReducedCost();
};

void PrintTree(const TreeAPI &tree);

void PrintTree(const TreeAPI &tree, const std::string &prefix, int nid);

void PrintBasisArc(const TreeAPI &tree);

void PrintNonBasisArc(const TreeAPI& tree);

}

#endif //NETWORK_SIMPLEX_TREE_H
