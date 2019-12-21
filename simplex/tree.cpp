//
// Created by LyxnO on 2019/11/16.
//

#include "tree.h"

#include <stack>
#include <iostream>

#include "utils.h"
#include "cycle.h"

namespace network {

using std::stack;

TreeAPI::TreeAPI(NodeIndex root,
                 bool debug) : root_(root),
                               debug_(debug),
                               num_seq_(500) {
}

int TreeAPI::InitBasisTree(const BasisArcKey &basis_arc, NodeIndex root) {
    basis_arc_.clear();
    for (auto &key: basis_arc) {
        auto src = key.first;
        auto dst = key.second;
        auto p_arc = GetArc(src, dst);
        basis_arc_.insert({key, p_arc});
        p_arc->SetStatus(FLOW_BASIS);
        GetNode(src)->AddNeighbor(dst);
        GetNode(dst)->AddNeighbor(src);
    }
    //Tree
    SetRoot(root);
    InitTreeStruct(root);
    UpdateDepth(root);
    //Flow
    CalcBasisFlow(basis_arc);
    CalcBasisPrice();
    UpdateReducedCost();
    return RET_SUCCESS;
}

int TreeAPI::InitArtificialBasis() {
    int num_nodes = GetNumNodes();
    NodeIndex artificial_id = num_nodes;
    AddNode(artificial_id, 0);
    BasisArcKey basis_arc;
    for (int i = 0; i < num_nodes; i++) {
        auto p_node = GetNode(i);
        auto nid = p_node->node_id_;
        if (nid == artificial_id) {
            continue;
        }
        if (p_node->supply_ > 0) {
            AddArtificialArc(nid, artificial_id);
            basis_arc.insert({nid, artificial_id});
        } else {
            AddArtificialArc(artificial_id, nid);
            basis_arc.insert({artificial_id, nid});
        }
    }
    InitBasisTree(basis_arc);
    return RET_SUCCESS;
}

int TreeAPI::SetRoot(NodeIndex root) {
    root_ = root;
    GetNode(root)->SetRoot();
    return RET_SUCCESS;
}

int TreeAPI::InitTreeStruct(NodeIndex root) {
    stack<NodeIndex> stk;
    stk.push(root);
    while (!stk.empty()) {
        auto pred_id = stk.top();
        stk.pop();
        auto p_node = GetNode(pred_id);
        if (p_node->neighbor_.empty()) {
            continue;
        }
        NodeIndex sibling = INVALID_NODE_ID;
        for (auto child_id: p_node->neighbor_) {
            //Oldest Child
            p_node->child_ = child_id;
            //Sibling
            auto p_child = GetNode(child_id);
            p_child->parent_ = pred_id;
            p_child->sibling_ = sibling;
            sibling = child_id;
            //Remove Parent in neighbor
            p_child->neighbor_.erase(pred_id);
            //Update stack
            stk.push(child_id);
        }
    }
    return RET_SUCCESS;
}

void TreeAPI::UpdateDepth(NodeIndex nid) {
    stack<NodeIndex> stk;
    stk.push(nid);
    while (!stk.empty()) {
        auto cur_id = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur_id);
        auto sibling = p_cur->sibling_;
        if (IsValidNode(sibling)) {
            auto p_sibling = GetNode(sibling);
            p_sibling->depth_ = p_cur->depth_;
            stk.push(sibling);
        }
        auto child = p_cur->child_;
        if (IsValidNode(child)) {
            auto p_child = GetNode(child);
            p_child->depth_ = p_cur->depth_ + 1;
            stk.push(child);
        }
    }
}

inline bool IsAnyOne(const int *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == 1) {
            return true;
        }
    }
    return false;
}

void TreeAPI::CalcBasisFlow(const BasisArcKey &basis_arc) {
    int num_node = GetNumNodes();
    int node_arc_cnt[num_node];
    int node_flow[num_node];
    for (int i = 0; i < num_node; i++) {
        node_arc_cnt[i] = 0;
        node_flow[i] = GetNode(i)->supply_;
    }
    for (auto key: basis_arc) {
        node_arc_cnt[key.first] += 1;
        node_arc_cnt[key.second] += 1;
    }
    std::set<ArcKey> proc_arc(basis_arc);
    do {
        if (!IsAnyOne(node_arc_cnt, num_node)) {
            break;
        }
        std::set<ArcKey> arc_del;
        for (auto &key: proc_arc) {
            auto p_arc = GetArc(key);
            auto src = p_arc->src_;
            auto dst = p_arc->dst_;
            if (node_arc_cnt[src] == 1) {
                p_arc->flow_ = node_flow[src];
                node_flow[dst] += p_arc->flow_;
                node_arc_cnt[dst] -= 1;
                node_flow[src] = 0;
                node_arc_cnt[src] = 0;
                arc_del.insert(key);
            } else if (node_arc_cnt[dst] == 1) {
                p_arc->flow_ = -node_flow[dst];
                node_flow[dst] -= p_arc->flow_;
                node_arc_cnt[dst] -= 1;
                node_flow[dst] = 0;
                node_arc_cnt[dst] = 0;
                arc_del.insert(key);
            }
        }
        for (auto &key: arc_del) {
            proc_arc.erase(key);
        }
    } while (true);
}

inline ArcPtr TreeAPI::GetBasisArc(int src, int dst) const {
    auto p_arc = FindHashMap(basis_arc_, {src, dst});
    if (p_arc != nullptr) {
        return p_arc;
    } else {
        return FindHashMap(basis_arc_, {dst, src});
    }
}

PriceType TreeAPI::CalcNodePrice(const NodePtr &refer, const NodePtr &unk) const {
    PriceType price = 0;
    auto p_arc = GetBasisArc(refer->node_id_, unk->node_id_);
    if (p_arc->src_ == refer->node_id_) {
        price = refer->price_ - p_arc->cost_;
    } else {
        price = refer->price_ + p_arc->cost_;
    }
    return price;
}

void TreeAPI::CalcBasisPrice() {
    auto p_root = GetNode(root_);
    p_root->price_ = 0;
    stack<NodeIndex> stk;
    stk.push(p_root->node_id_);
    while (!stk.empty()) {
        auto cur_id = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur_id);
        if (IsValidNode(p_cur->sibling_)) {
            auto p_sibling = GetNode(p_cur->sibling_);
            auto p_father = GetNode(p_sibling->parent_);
            p_sibling->price_ = CalcNodePrice(p_father, p_sibling);
            stk.push(p_sibling->node_id_);
        }
        if (IsValidNode(p_cur->child_)) {
            auto p_child = GetNode(p_cur->child_);
            p_child->price_ = CalcNodePrice(p_cur, p_child);
            stk.push(p_child->node_id_);
        }
    }
}

void TreeAPI::UpdateReducedCost() {
    for (int i = 0; i < GetNumArcs(); i++) {
        auto p_arc = GetArc(i);
        p_arc->reduced_cost_ = CalcReducedCost(p_arc);
    }
}

PriceType TreeAPI::CalcReducedCost(const ArcPtr &p_arc) const {
    PriceType reduced_cost = 0;
    if (p_arc->IsStatusBasis()) {
        reduced_cost = 0;
    } else {
        auto p_src = GetNode(p_arc->src_);
        auto p_dst = GetNode(p_arc->dst_);
        reduced_cost = p_arc->cost_ - (p_src->price_ - p_dst->price_);
    }
    return reduced_cost;
}

PriceType TreeAPI::GetTotalCost() const {
    PriceType cost = 0;
    for (int i = 0; i < GetNumArcs(); i++) {
        auto p_arc = GetArc(i);
        if (p_arc->IsStatusLower()) {
            continue;
        }
        cost += p_arc->cost_ * p_arc->flow_;
    }
    return cost;
}

NodePtr TreeAPI::FindNodeJoint(NodeIndex src, NodeIndex dst) {
    auto p_src = GetNode(src);
    auto p_dst = GetNode(dst);
    do {
        if (p_src == nullptr || p_dst == nullptr) {
            return nullptr;
        } else if (p_src->is_root_) {
            return p_src;
        } else if (p_dst->is_root_) {
            return p_dst;
        } else if (p_src->node_id_ == p_dst->node_id_) {
            return p_src;
        }
        if (p_src->depth_ > p_dst->depth_) {
            p_src = GetNode(p_src->parent_);
        } else {
            p_dst = GetNode(p_dst->parent_);
        }
    } while (true);
    return nullptr;
}



ArcPtr TreeAPI::FindArcInFirst() const {
    for (int i = 0; i < GetNumArcs(); i++) {
        auto p_arc = GetArc(i);
        if (p_arc->IsStatusBasis()) {
            continue;
        }
        PriceType reduced_cost = CalcReducedCost(p_arc);
        if (reduced_cost < 0 && p_arc->IsStatusLower()) {
            return p_arc;
        } else if (reduced_cost > 0 && p_arc->IsStatusUpper()) {
            return p_arc;
        }
    }
    return nullptr;
}

inline void UpdateArcDirection(const ArcPtr &refer, ArcPtr &unk) {
    if (refer->src_ == unk->src_ || refer->dst_ == unk->dst_) {
        unk->direction_ = (refer->direction_ == D_POS) ? D_NEG : D_POS;
    } else {
        unk->direction_ = refer->direction_;
    }
}

void TreeAPI::UpdateCyclePath(ArcPtr &arc_in, NodeIndex nid, std::vector<ArcPtr> &path) {
    auto last_arc = arc_in;
    auto last_node = GetNode(nid);
    while (last_node->node_id_ != cycle_.node_joint) {
        auto p_node = GetNode(last_node->parent_);
        auto p_arc = GetBasisArc(p_node->node_id_, last_node->node_id_);
        UpdateArcDirection(last_arc, p_arc);
        path.push_back(p_arc);
        last_arc = p_arc;
        last_node = p_node;
    }
}

void TreeAPI::UpdateCyclePath() {
    auto p_arc_in = cycle_.arc_in;
    p_arc_in->direction_ = (p_arc_in->IsStatusUpper()) ? D_NEG : D_POS;
    //add entering arc to path
    cycle_.path.push_back(p_arc_in);
    //path from src
    auto src = p_arc_in->src_;
    UpdateCyclePath(p_arc_in, src, cycle_.path);
    //path from dst
    auto dst = p_arc_in->dst_;
    UpdateCyclePath(p_arc_in, dst, cycle_.path);
}

inline int GetFlowCapacity(const ArcPtr &p_arc) {
    if (p_arc->direction_ == D_POS) {
        return p_arc->capacity_ - p_arc->flow_;
    } else {
        return p_arc->flow_;
    }
}

inline ArcPtr FindMinFlow(const std::vector<ArcPtr> &path) {
    FlowType min_flow = MAX_CAPACITY;
    ArcPtr min_arc;
    for (const auto &p_arc: path) {
        auto cur_flow = GetFlowCapacity(p_arc);
        if (cur_flow < min_flow) {
            min_flow = cur_flow;
            min_arc = p_arc;
        }
    }
    return min_arc;
}

//entering arc can be leaving arc too
ArcPtr TreeAPI::GetMinFlowArc(const ArcPtr &arc_in) {
    auto min_arc = FindMinFlow(cycle_.path);
    if (arc_in->arc_id_ == min_arc->arc_id_ && arc_in->is_artificial_) {
        min_arc = nullptr;
    }
    return min_arc;
}

ArcPtr TreeAPI::FindArcOut(const ArcPtr &arc_in) {
    auto src = arc_in->src_;
    auto dst = arc_in->dst_;
    cycle_.arc_in = arc_in;
    //find joint
    auto p_node = FindNodeJoint(src, dst);
    cycle_.node_joint = p_node->node_id_;
    //update cycle path
    UpdateCyclePath();
    //find min flow arc
    auto min_arc = GetMinFlowArc(arc_in);
    cycle_.arc_out = min_arc;
    return min_arc;
}

inline void UpdateArcFlow(ArcPtr &arc, FlowType flow) {
    if (arc->direction_ == D_POS) {
        arc->flow_ += flow;
    } else {
        arc->flow_ -= flow;
    }
}

inline void UpdatePathFlow(std::vector<ArcPtr> &path, FlowType flow) {
    for (auto &arc: path) {
        UpdateArcFlow(arc, flow);
    }
}

void TreeAPI::UpdateCycleFlow(ArcPtr &arc_in, ArcPtr &arc_out) {
    int flow = GetFlowCapacity(arc_out);
    UpdatePathFlow(cycle_.path, flow);
}

void TreeAPI::UpdateBasisArc(ArcPtr &arc_in, ArcPtr &arc_out) {
    arc_in->SetStatus(FLOW_BASIS);
    arc_out->SetStatus(arc_out->flow_ == 0 ? FLOW_LOWER : FLOW_UPPER);
    basis_arc_.insert({GetArcKey(arc_in), arc_in});
    basis_arc_.erase(GetArcKey(arc_out));
}

bool TreeAPI::IsNodeInRoot(NodeIndex nid) const {
    auto p_node = GetNode(nid);
    while (p_node->node_id_ != cycle_.node_joint) {
        if (p_node->node_id_ == cycle_.arc_out->src_ || p_node->node_id_ == cycle_.arc_out->dst_) {
            return false;
        }
        p_node = GetNode(p_node->parent_);
    }
    return true;
}

void TreeAPI::UpdateTree(ArcPtr &arc_in, ArcPtr &arc_out) {
    auto out_src = GetNode(arc_out->src_);
    auto out_dst = GetNode(arc_out->dst_);
    NodePtr out_upd;
    if (out_src->parent_ == out_dst->node_id_) {
        out_upd = out_src;
    } else {
        out_upd = out_dst;
    }
    NodePtr in_upd;
    NodePtr in_root;
    if (IsNodeInRoot(arc_in->src_)) {
        in_root = GetNode(arc_in->src_);
        in_upd = GetNode(arc_in->dst_);
    } else {
        in_root = GetNode(arc_in->dst_);
        in_upd = GetNode(arc_in->src_);
    }
    UpdateTreeStruct(in_root, in_upd, out_upd);
    //update depth & price
    auto reduced_cost = CalcReducedCost(arc_in);
    auto delta = (in_upd->node_id_ == arc_in->src_) ? reduced_cost : -reduced_cost;
    in_upd->price_ += delta;
    in_upd->depth_ = in_root->depth_ + 1;
    UpdateDepthAndPrice(in_upd->node_id_, delta);
}

void TreeAPI::UpdateDepthAndPrice(NodeIndex nid, PriceType delta) {
    stack<NodeIndex> stk;
    stk.push(nid);
    while (!stk.empty()) {
        auto cur_id = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur_id);
        auto sibling = p_cur->sibling_;
        if (IsValidNode(sibling)) {
            auto p_sibling = GetNode(sibling);
            p_sibling->depth_ = p_cur->depth_;
            p_sibling->price_ += delta;
            stk.push(sibling);
        }
        auto child = p_cur->child_;
        if (IsValidNode(child)) {
            auto p_child = GetNode(child);
            p_child->depth_ = p_cur->depth_ + 1;
            p_child->price_ += delta;
            stk.push(child);
        }
    }
}

void TreeAPI::UpdateTreeStruct(NodePtr &in_root, NodePtr &in_upd, NodePtr &out_upd) {
    AddChild(in_root, in_upd);
    NodePtr cur_node = in_upd;
    auto new_father = in_root->node_id_;
    while (cur_node->node_id_ != out_upd->node_id_) {
        auto p_old_father = GetNode(cur_node->parent_);
        AddChild(cur_node, p_old_father);
        DelChild(p_old_father, cur_node);
        cur_node->parent_ = new_father;
        new_father = cur_node->node_id_;
        cur_node = p_old_father;
    }
    auto out_root = GetNode(out_upd->parent_);
    DelChild(out_root, out_upd);
    out_upd->parent_ = new_father;
}

void TreeAPI::AddChild(NodePtr &node, NodePtr &child) {
    if (!IsValidNode(node->child_)) {
        node->child_ = child->node_id_;
        return;
    }
    auto cur = GetNode(node->child_);
    while (IsValidNode(cur->sibling_)) {
        cur = GetNode(cur->sibling_);
    }
    cur->sibling_ = child->node_id_;
}

void TreeAPI::DelChild(NodePtr &node, NodePtr &child) {
    if (node->child_ == child->node_id_) {
        node->child_ = child->sibling_;
        child->sibling_ = INVALID_NODE_ID;
        return;
    }

    auto cur = node->child_;
    while (IsValidNode(cur)) {
        auto p_cur = GetNode(cur);
        if (p_cur->sibling_ == child->node_id_) {
            p_cur->sibling_ = child->sibling_;
            child->sibling_ = INVALID_NODE_ID;
            break;
        }
        cur = p_cur->sibling_;
    }
}

void TreeAPI::Pivot(ArcPtr &arc_in, ArcPtr &arc_out) {
    //Update Flow
    UpdateCycleFlow(arc_in, arc_out);
    if (arc_in->arc_id_ == arc_out->arc_id_) {
        arc_out->SetStatus(arc_out->flow_ == 0 ? FLOW_LOWER : FLOW_UPPER);
        return;
    }
    //Update Tree
    UpdateTree(arc_in, arc_out);
    //Update Basis
    UpdateBasisArc(arc_in, arc_out);
}

int TreeAPI::RunSimplex(int max_iter) {
    int iter = 1;
    do {
        cycle_.Clear();
//        auto arc_in = FindArcInFirst();
        auto arc_in = FindArcInBySeq();
        if (arc_in == nullptr) {
            printf("Problem solved.\n");
            std::cout << "Iteration: " << iter
                      << " TotalCost: " << GetTotalCost() << std::endl;
            break;
        }
        auto arc_out = FindArcOut(arc_in);
        if (arc_out == nullptr) {
            printf("Problem unbound.\n");
            std::cout << "Iteration: " << iter
                      << " TotalCost: " << GetTotalCost() << std::endl;
            break;
        }
        if (debug_) {
            std::cout << "Iteration: " << iter
                      << " TotalCost: " << GetTotalCost() << std::endl;
            std::cout << "ArcIn: " << *arc_in
                      << " reduced: " << CalcReducedCost(arc_in)
                      << " volume: " << GetFlowCapacity(arc_in)
                      << std::endl;
            std::cout << "ArcOut: " << *arc_out
                      << " reduced: " << CalcReducedCost(arc_out)
                      << " volume: " << GetFlowCapacity(arc_out)
                      << std::endl;
            std::cout << "Joint: " << *GetNode(cycle_.node_joint) << std::endl;
        }
        Pivot(arc_in, arc_out);
        if (iter >= max_iter) {
            printf("Problem failed.\n");
            std::cout << "Iteration: " << iter
                      << " TotalCost: " << GetTotalCost() << std::endl;
            break;
        }
        iter += 1;
    } while (true);
    return RET_SUCCESS;
}

ArcPtr TreeAPI::FindArcInBySeq() {
    int num_arcs = GetNumArcs();
    int seq = 0;
    ArcPtr max_arc;
    PriceType max_cost = -1;
    for (int t = 0; t < num_arcs; t++) {
        int idx = seq + offset_;
        if (idx >= num_arcs) {
            seq = 0;
            offset_ = 0;
            idx = 0;
        }
        auto p_arc = GetArc(idx);
        auto reduced_cost = CalcReducedCost(p_arc);
        if (reduced_cost < 0 && p_arc->IsStatusLower() && -reduced_cost > max_cost) {
            max_arc = p_arc;
            max_cost = -reduced_cost;
        } else if (reduced_cost > 0 && p_arc->IsStatusUpper() && reduced_cost > max_cost) {
            max_arc = p_arc;
            max_cost = reduced_cost;
        }
        seq += 1;
        if (seq >= num_seq_) {
            if (max_arc == nullptr) {
                seq = 0;
                offset_ += num_seq_;
            } else {
                break;
            }
        }
    }
    return max_arc;
}

void PrintTree(const TreeAPI &tree) {
    PrintTree(tree, "", tree.root_);
}

void PrintTree(const TreeAPI &tree, const std::string &prefix, int nid) {
    if (!tree.IsValidNode(nid)) {
        return;
    }
    auto p_node = tree.GetNode(nid);
    printf("%s(%d %d %d %ld)\n",
           prefix.c_str(),
           p_node->node_id_,
           p_node->depth_,
           p_node->supply_,
           p_node->price_
    );
    auto prefix_child = prefix + "  ";
    PrintTree(tree, prefix_child, p_node->child_);
    PrintTree(tree, prefix, p_node->sibling_);
}

void PrintArc() {
    printf("src\tdst\tcap\tflow\tdir\tcost\tred\n");
}

void PrintArc(const ArcPtr &p_arc) {
    int capacity = (p_arc->capacity_ == MAX_CAPACITY) ? 0 : p_arc->capacity_;
    printf("%d\t%d\t%d\t%d\t%d\t%ld\t%ld\n",
           p_arc->src_,
           p_arc->dst_,
           capacity,
           p_arc->flow_,
           p_arc->direction_,
           p_arc->cost_,
           p_arc->reduced_cost_
    );
}


void PrintBasisArc(const TreeAPI &tree) {
    PrintArc();
    for (int i = 0; i < tree.GetNumArcs(); i++) {
        auto p_arc = tree.GetArc(i);
        if (p_arc->IsStatusBasis()) {
            PrintArc(p_arc);
        }
    }
}

void PrintNonBasisArc(const TreeAPI &tree) {
    for (int i = 0; i < tree.GetNumArcs(); i++) {
        auto p_arc = tree.GetArc(i);
        if (!p_arc->IsStatusBasis()) {
            PrintArc(p_arc);
        }
    }
}

void PrintArcFlow(const TreeAPI &tree) {
    PrintArc();
    for (int i = 0; i < tree.GetNumArcs(); i++) {
        auto p_arc = tree.GetArc(i);
        if (p_arc->flow_ > 0) {
            PrintArc(p_arc);
        }
    }
}

void PrintTreeAndBasis(const TreeAPI &tree) {
    printf("Tree\n");
    PrintTree(tree);
    printf("Basis\n");
    PrintBasisArc(tree);
    printf("NonBasis\n");
    PrintNonBasisArc(tree);
    printf("TotalCost=%ld\n", tree.GetTotalCost());
}

}
