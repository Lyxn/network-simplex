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
typedef std::set<ArcPtr> ArcSet;

class TreeAPI : public Network {
public:
    TreeAPI() = default;

    ~TreeAPI() override = default;

    int InitBasisTree(const BasisArc &basis_arc, int root = 0);

    int InitArtificialBasis();

    double GetTotalCost() const;

    bool IsBasisArc(const ArcKey &key) const;

    ArcPtr GetBasisArc(int src, int dst) const;

    int SetRoot(int root);

    int InitTreeStruct(int root);

    void UpdateDepth(int nid);

    NodePtr FindNodeJoint(int src, int dst);

    std::set<int> FindChildren(NodePtr &node);

    int RunSimplex(int max_iter = 100);

public:
    int root_{};
    BasisArc basis_arc_;
    Cycle cycle_{};
    ArcSet candidate_arcs_; //TODO collect the candidate arcs
    bool debug_{};

private:
    void CalcBasisFlow(const BasisArc &basis_arc);

    double CalcNodePrice(const NodePtr &refer, const NodePtr &unk) const;

    void CalcBasisPrice();

    void UpdateReducedCost();

    double CalcReducedCost(const ArcPtr &p_arc) const;

    ArcPtr FindArcIn() const;

    //Cycle
    void UpdateCyclePath();

    void UpdateCyclePath(ArcPtr &arc_in, int nid, std::vector<ArcPtr> &path);

    ArcPtr GetMinFlowArc();

    ArcPtr FindArcOut(const ArcPtr &arc_in);

    //Pivot
    void Pivot(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateCycleFlow(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateBasisArc(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateNodePrice(const std::set<int> &tree_upd, ArcPtr &arc_in);

    void UpdateTree(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateTreeStruct(const std::set<int> &tree_upd, ArcPtr &arc_in, NodePtr &out_upd);

    void AddChild(NodePtr &node, NodePtr &child);

    void DelChild(NodePtr &node, NodePtr &child);
};

void PrintTree(const TreeAPI &tree);

void PrintTree(const TreeAPI &tree, const std::string &prefix, int nid);

void PrintBasisArc(const TreeAPI &tree);

void PrintNonBasisArc(const TreeAPI &tree);

void PrintTreeAndBasis(const TreeAPI &tree);

void PrintArcFlow(const TreeAPI &tree);

}

#endif //NETWORK_SIMPLEX_TREE_H
