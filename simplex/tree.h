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

typedef std::set<ArcKey> BasisArcKey;
typedef std::set<ArcPtr> ArcSet;

class TreeAPI : public Network {
public:
    explicit TreeAPI(NodeIndex root = 0, bool debug = false);

    ~TreeAPI() override = default;

    NodeIndex InitBasisTree(const BasisArcKey &basis_arc, NodeIndex root = 0);

    NodeIndex InitArtificialBasis();

    PriceType GetTotalCost() const;

    ArcPtr GetBasisArc(NodeIndex src, NodeIndex dst) const;

    NodeIndex SetRoot(NodeIndex root);

    NodeIndex InitTreeStruct(NodeIndex root);

    void UpdateDepth(NodeIndex nid);

    NodePtr FindNodeJoint(NodeIndex src, NodeIndex dst);

    int RunSimplex(int max_iter = 100000);

public:
    NodeIndex root_{};
    ArcPtrMap basis_arc_;
    Cycle cycle_{};
    ArcSet candidate_arcs_; //TODO collect the candidate arcs
    bool debug_{};
    int num_seq_{};
    int offset_{};

private:
    void CalcBasisFlow(const BasisArcKey &basis_arc);

    PriceType CalcNodePrice(const NodePtr &refer, const NodePtr &unk) const;

    void CalcBasisPrice();

    void UpdateReducedCost();

    PriceType CalcReducedCost(const ArcPtr &p_arc) const;

    ArcPtr FindArcInFirst() const;

    ArcPtr FindArcInBySeq();

    //Cycle
    void UpdateCyclePath();

    void UpdateCyclePath(ArcPtr &arc_in, NodeIndex nid, std::vector<ArcPtr> &path);

    ArcPtr GetMinFlowArc(const ArcPtr &arc_in);

    ArcPtr FindArcOut(const ArcPtr &arc_in);

    bool IsNodeInRoot(int nid) const;

    //Pivot
    void Pivot(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateCycleFlow(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateBasisArc(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateTree(ArcPtr &arc_in, ArcPtr &arc_out);

    void UpdateDepthAndPrice(NodeIndex nid, PriceType delta);

    void UpdateTreeStruct(NodePtr &in_root, NodePtr &in_upd, NodePtr &out_upd);

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
