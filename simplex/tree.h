//
// Created by LyxnO on 2019/11/16.
//

#ifndef NETWORK_SIMPLEX_TREE_H
#define NETWORK_SIMPLEX_TREE_H

#include <set>
#include <vector>

#include "network.h"
#include "cycle.h"

namespace network {

typedef std::set<ArcKey> BasisArc;

class TreeAPI : public Network {
public:
    TreeAPI() = default;

    ~TreeAPI() override = default;

    int InitBasisTree(const BasisArc &basis_arc, int root = 0);

    int InitArtificialBasis();

    double GetTotalCost() const;

    double GetBasisCost() const;

    bool IsBasisArc(const ArcKey &key) const;

    ArcPtr GetBasisArc(int src, int dst);

    int SetRoot(int root);

    int InitTreeStruct(int root);

    void UpdateDepth(int nid);

    NodePtr FindNodeJoint(int src, int dst);

    int RunSimplex(int max_iter = 1);

public:
    int root_{};
    BasisArc basis_arc_;
    Cycle cycle_{};

private:
    void CalcBasisFlow(const BasisArc &basis_arc);

    void CalcNodePrice(NodePtr refer, NodePtr unk);

    void CalcBasisPrice();

    void CalcReducedCost();

    ArcPtr FindArcIn() const;

    void UpdateCyclePath();

    void UpdateCyclePath(ArcPtr &p_arc_in, int nid, std::vector<ArcPtr> &path);

    ArcPtr GetMinFlowArc();

    ArcPtr FindArcOut(const ArcPtr &arc_in);

};

void PrintTree(const TreeAPI &tree);

void PrintTree(const TreeAPI &tree, const std::string &prefix, int nid);

void PrintBasisArc(const TreeAPI &tree);

void PrintNonBasisArc(const TreeAPI &tree);

}

#endif //NETWORK_SIMPLEX_TREE_H
