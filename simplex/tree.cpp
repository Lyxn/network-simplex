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

int TreeAPI::InitBasisTree(const BasisArc &basis_arc, int root) {
    basis_arc_.clear();
    for (auto &key: basis_arc) {
        auto src = key.first;
        auto dst = key.second;
        auto p_arc = GetArc(src, dst);
        basis_arc_.insert(key);
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
    int artificial_id = GetNodeNum();
    AddNode(artificial_id, 0);
    BasisArc basis_arc;
    for (auto &it: nodes_) {
        int nid = it.first;
        auto p_node = it.second;
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

int TreeAPI::SetRoot(int root) {
    root_ = root;
    GetNode(root)->set_root();
    return RET_SUCCESS;
}

int TreeAPI::InitTreeStruct(int root) {
    stack<int> stk;
    stk.push(root);
    while (!stk.empty()) {
        auto pred_id = stk.top();
        stk.pop();
        auto p_node = GetNode(pred_id);
        if (p_node->neighbor_.empty()) {
            continue;
        }
        bool is_first = true;
        int brother = INVALID_NODE_ID;
        for (auto child_id: p_node->neighbor_) {
            //Oldest Son
            p_node->son_ = child_id;
            //Brother
            auto p_child = GetNode(child_id);
            p_child->father_ = pred_id;
            p_child->brother_ = brother;
            brother = child_id;
            //Remove Parent in neighbor
            p_child->neighbor_.erase(pred_id);
            //Update stack
            stk.push(child_id);
        }
    }
    return RET_SUCCESS;
}

void TreeAPI::UpdateDepth(int nid) {
    stack<int> stk;
    stk.push(nid);
    while (!stk.empty()) {
        auto cur_id = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur_id);
        int son = p_cur->son_;
        int brother = p_cur->brother_;
        auto p_brother = GetNode(brother);
        if (p_brother != nullptr) {
            p_brother->depth_ = p_cur->depth_;
            stk.push(brother);
        }
        auto p_son = GetNode(son);
        if (p_son != nullptr) {
            p_son->depth_ = p_cur->depth_ + 1;
            stk.push(son);
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

void TreeAPI::CalcBasisFlow(const BasisArc &basis_arc) {
    int num_node = GetNodeNum();
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
            int src = p_arc->GetSrcId();
            int dst = p_arc->GetDstId();
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

ArcPtr TreeAPI::GetBasisArc(int src, int dst) const {
    auto p_arc = GetArc(src, dst);
    if (p_arc != nullptr && p_arc->IsStatusBasis()) {
        return p_arc;
    } else {
        return GetArc(dst, src);
    }
}

double TreeAPI::CalcNodePrice(const NodePtr &refer, const NodePtr &unk) const {
    double price = 0;
    auto p_arc = GetBasisArc(refer->node_id_, unk->node_id_);
    if (p_arc->GetSrcId() == refer->node_id_) {
        price = refer->price_ - p_arc->cost_;
    } else {
        price = refer->price_ + p_arc->cost_;
    }
    return price;
}

void TreeAPI::CalcBasisPrice() {
    auto p_root = GetNode(root_);
    p_root->price_ = 0;
    stack<int> stk;
    stk.push(p_root->node_id_);
    while (!stk.empty()) {
        auto cur_id = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur_id);
        auto p_brother = GetNode(p_cur->brother_);
        if (p_brother != nullptr) {
            auto p_father = GetNode(p_brother->father_);
            p_brother->price_ = CalcNodePrice(p_father, p_brother);
            stk.push(p_brother->node_id_);
        }
        auto p_son = GetNode(p_cur->son_);
        if (p_son != nullptr) {
            p_son->price_ = CalcNodePrice(p_cur, p_son);
            stk.push(p_son->node_id_);
        }
    }
}

bool TreeAPI::IsBasisArc(const ArcKey &key) const {
    return HasValue(basis_arc_, key);
}

void TreeAPI::UpdateReducedCost() {
    for (auto &item: arc_idx_) {
        auto p_arc = item.second;
        p_arc->reduced_cost_ = CalcReducedCost(p_arc);
    }
}

double TreeAPI::CalcReducedCost(const ArcPtr &p_arc) const {
    double reduced_cost = 0;
    if (p_arc->IsStatusBasis()) {
        reduced_cost = 0;
    } else {
        auto p_src = GetNode(p_arc->GetSrcId());
        auto p_dst = GetNode(p_arc->GetDstId());
        reduced_cost = p_arc->cost_ - (p_src->price_ - p_dst->price_);
    }
    return reduced_cost;
}

double TreeAPI::GetTotalCost() const {
    double cost = 0;
    for (auto &it: arc_idx_) {
        auto p_arc = it.second;
        if (p_arc->IsStatusLower()) {
            continue;
        }
        cost += p_arc->cost_ * p_arc->flow_;
    }
    return cost;
}

NodePtr TreeAPI::FindNodeJoint(int src, int dst) {
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
            p_src = GetNode(p_src->father_);
        } else {
            p_dst = GetNode(p_dst->father_);
        }
    } while (true);
    return nullptr;
}

std::set<int> TreeAPI::FindChildren(NodePtr &node) {
    std::set<int> successors{node->node_id_};
    stack<int> stk;
    auto son = GetNode(node->son_);
    if (son != nullptr) {
        stk.push(son->node_id_);
    }
    while (!stk.empty()) {
        int cur = stk.top();
        stk.pop();
        auto p_cur = GetNode(cur);
        if (p_cur != nullptr) {
            successors.insert(p_cur->node_id_);
            stk.push(p_cur->son_);
            stk.push(p_cur->brother_);
        }
    }
    return successors;
}

void PrintNode(const NodePtr &p_node) {
    printf("%d\t%d\t%d\t%0.1f\n",
           p_node->node_id_,
           p_node->depth_,
           p_node->supply_,
           p_node->price_
    );
}

void PrintTree(const TreeAPI &tree) {
    PrintTree(tree, "", tree.root_);
}

void PrintTree(const TreeAPI &tree, const std::string &prefix, int nid) {
    auto p_node = tree.GetNode(nid);
    if (p_node == nullptr) {
        return;
    }
    printf("%s(%d %d %d %0.1f)\n",
           prefix.c_str(),
           p_node->node_id_,
           p_node->depth_,
           p_node->supply_,
           p_node->price_
    );
    auto prefix_son = prefix + "  ";
    PrintTree(tree, prefix_son, p_node->son_);
    PrintTree(tree, prefix, p_node->brother_);
}

void PrintArc() {
    printf("src\tdst\tcap\tflow\tdir\tcost\tred\n");
}

void PrintArc(const ArcPtr &p_arc) {
    int capacity = (p_arc->capacity_ == MAX_CAPACITY) ? 0 : p_arc->capacity_;
    printf("%d\t%d\t%d\t%d\t%d\t%0.1f\t%0.1f\n",
           p_arc->src_id_,
           p_arc->dst_id_,
           capacity,
           p_arc->flow_,
           p_arc->direction_,
           p_arc->cost_,
           p_arc->reduced_cost_
    );
}

void PrintBasisArc(const TreeAPI &tree) {
    PrintArc();
    for (auto &key: tree.basis_arc_) {
        auto p_arc = tree.GetArc(key);
        PrintArc(p_arc);
    }
}

void PrintArcFlow(const TreeAPI &tree) {
    PrintArc();
    for (auto &it: tree.arc_idx_) {
        auto p_arc = it.second;
        if (p_arc->flow_ == 0) {
            continue;
        }
        PrintArc(p_arc);
    }
}

void PrintNonBasisArc(const TreeAPI &tree) {
    for (auto &it: tree.arc_idx_) {
        if (tree.IsBasisArc(it.first)) {
            continue;
        }
        PrintArc(it.second);
    }
}

void PrintTreeAndBasis(const TreeAPI &tree) {
    printf("Tree\n");
    PrintTree(tree);
    printf("Basis\n");
    PrintBasisArc(tree);
    printf("NonBasis\n");
    PrintNonBasisArc(tree);
    printf("TotalCost=%f\n", tree.GetTotalCost());
}

ArcPtr TreeAPI::FindArcIn() const {
    for (auto &it: arc_idx_) {
        if (IsBasisArc(it.first)) {
            continue;
        }
        auto p_arc = it.second;
        double reduced_cost = CalcReducedCost(p_arc);
        if (reduced_cost < 0 && p_arc->IsStatusLower()) {
            return p_arc;
        } else if (reduced_cost > 0 && p_arc->IsStatusUpper()) {
            return p_arc;
        }
    }
    return nullptr;
}

inline void UpdateArcDirection(const ArcPtr &refer, ArcPtr &unk) {
    if (refer->GetSrcId() == unk->GetSrcId() || refer->GetDstId() == unk->GetDstId()) {
        unk->direction_ = (refer->direction_ == D_POS) ? D_NEG : D_POS;
    } else {
        unk->direction_ = refer->direction_;
    }
}

void TreeAPI::UpdateCyclePath(ArcPtr &arc_in, int nid, std::vector<ArcPtr> &path) {
    auto last_arc = arc_in;
    auto last_node = GetNode(nid);
    while (last_node->node_id_ != cycle_.node_joint) {
        auto p_node = GetNode(last_node->father_);
        auto p_arc = GetBasisArc(p_node->node_id_, last_node->node_id_);
        UpdateArcDirection(last_arc, p_arc);
        path.push_back(p_arc);
        last_arc = p_arc;
        last_node = p_node;
    }
}

void TreeAPI::UpdateCyclePath() {
    auto p_arc_in = GetArc(cycle_.arc_in);
    p_arc_in->direction_ = (p_arc_in->IsStatusUpper()) ? D_NEG : D_POS;
    //path from src
    int src = p_arc_in->GetSrcId();
    UpdateCyclePath(p_arc_in, src, cycle_.path_src);
    //path from dst
    int dst = p_arc_in->GetDstId();
    UpdateCyclePath(p_arc_in, dst, cycle_.path_dst);
}

inline int GetFlowCapacity(const ArcPtr &p_arc) {
    if (p_arc->direction_ == D_POS) {
        return p_arc->capacity_ - p_arc->flow_;
    } else {
        return p_arc->flow_;
    }
}

inline ArcPtr FindMinFlow(const std::vector<ArcPtr> &path) {
    int min_flow = MAX_CAPACITY;
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

ArcPtr TreeAPI::GetMinFlowArc(const ArcPtr &arc_in) {
    auto min_arc_src = FindMinFlow(cycle_.path_src);
    auto min_arc_dst = FindMinFlow(cycle_.path_dst);
    ArcPtr min_arc;
    if (min_arc_src == nullptr) {
        min_arc = min_arc_dst;
    } else if (min_arc_dst == nullptr) {
        min_arc = min_arc_src;
    } else if (GetFlowCapacity(min_arc_src) > GetFlowCapacity(min_arc_dst)) {
        min_arc = min_arc_dst;
    } else {
        min_arc = min_arc_src;
    }
    //entering arc is also leaving arc
    if (GetFlowCapacity(min_arc) > GetFlowCapacity(arc_in)) {
        min_arc = arc_in;
    }
    return min_arc;
}

ArcPtr TreeAPI::FindArcOut(const ArcPtr &arc_in) {
    int src = arc_in->GetSrcId();
    int dst = arc_in->GetDstId();
    cycle_.arc_in = {src, dst};
    //find joint
    auto p_node = FindNodeJoint(src, dst);
    cycle_.node_joint = p_node->node_id_;
    //update cycle path
    UpdateCyclePath();
    //find min flow arc
    auto min_arc = GetMinFlowArc(arc_in);
    cycle_.arc_out = (min_arc == nullptr) ? INVALID_ARC_KEY : GetArcKey(min_arc);
    return min_arc;
}

inline void UpdateArcFlow(ArcPtr &arc, int flow) {
    if (arc->direction_ == D_POS) {
        arc->flow_ += flow;
    } else {
        arc->flow_ -= flow;
    }
}

inline void UpdatePathFlow(std::vector<ArcPtr> &path, int flow) {
    for (auto &arc: path) {
        UpdateArcFlow(arc, flow);
    }
}

void TreeAPI::UpdateCycleFlow(ArcPtr &arc_in, ArcPtr &arc_out) {
    int flow = GetFlowCapacity(arc_out);
    UpdateArcFlow(arc_in, flow);
    UpdatePathFlow(cycle_.path_src, flow);
    UpdatePathFlow(cycle_.path_dst, flow);
}

void TreeAPI::UpdateBasisArc(ArcPtr &arc_in, ArcPtr &arc_out) {
    arc_in->status_ = FLOW_BASIS;
    arc_out->status_ = (arc_out->flow_ == 0) ? FLOW_LOWER : FLOW_UPPER;
    basis_arc_.insert(GetArcKey(arc_in));
    basis_arc_.erase(GetArcKey(arc_out));
}

void TreeAPI::UpdateNodePrice(const std::set<int> &tree_upd, ArcPtr &arc_in) {
    double reduced_cost = CalcReducedCost(arc_in);
    double delta = HasValue(tree_upd, arc_in->GetSrcId()) ? reduced_cost : -reduced_cost;
    for (auto nid: tree_upd) {
        auto p_node = GetNode(nid);
        p_node->price_ += delta;
    }
}

void TreeAPI::UpdateTree(ArcPtr &arc_in, ArcPtr &arc_out) {
    auto out_src = GetNode(arc_out->GetSrcId());
    auto out_dst = GetNode(arc_out->GetDstId());
    NodePtr out_upd;
    if (out_src->father_ == out_dst->node_id_) {
        out_upd = out_src;
    } else {
        out_upd = out_dst;
    }
    auto tree_upd = FindChildren(out_upd);
    if (debug_) {
        printf("ArcOut(%d, %d) Update Node=%d TreeSize=%d\n",
               arc_out->GetSrcId(), arc_out->GetDstId(), out_upd->node_id_, tree_upd.size());
    }
    UpdateNodePrice(tree_upd, arc_in);
    UpdateTreeStruct(tree_upd, arc_in, out_upd);
}

void TreeAPI::UpdateTreeStruct(const std::set<int> &tree_upd, ArcPtr &arc_in, NodePtr &out_upd) {
    NodePtr in_upd;
    NodePtr in_root;
    if (HasValue(tree_upd, arc_in->GetSrcId())) {
        in_upd = GetNode(arc_in->GetSrcId());
        in_root = GetNode(arc_in->GetDstId());
    } else {
        in_upd = GetNode(arc_in->GetDstId());
        in_root = GetNode(arc_in->GetSrcId());
    }
    AddChild(in_root, in_upd);
    NodePtr cur_node = in_upd;
    int new_father = in_root->node_id_;
    while (cur_node->node_id_ != out_upd->node_id_) {
        auto p_old_father = GetNode(cur_node->father_);
        AddChild(cur_node, p_old_father);
        DelChild(p_old_father, cur_node);
        cur_node->father_ = new_father;
        new_father = cur_node->node_id_;
        cur_node = p_old_father;
    }
    auto out_root = GetNode(out_upd->father_);
    DelChild(out_root, out_upd);
    out_upd->father_ = new_father;
    //update depth
    in_upd->depth_ = in_root->depth_ + 1;
    UpdateDepth(in_upd->node_id_);
}

void TreeAPI::AddChild(NodePtr &node, NodePtr &child) {
    auto son = GetNode(node->son_);
    if (son == nullptr) {
        node->son_ = child->node_id_;
        return;
    }
    while (son->brother_ != INVALID_NODE_ID) {
        son = GetNode(son->brother_);
    }
    son->brother_ = child->node_id_;
}

void TreeAPI::DelChild(NodePtr &node, NodePtr &child) {
    auto son = GetNode(node->son_);
    if (son->node_id_ == child->node_id_) {
        node->son_ = child->brother_;
        child->brother_ = INVALID_NODE_ID;
        return;
    }

    while (son != nullptr) {
        if (son->brother_ == child->node_id_) {
            son->brother_ = child->brother_;
            child->brother_ = INVALID_NODE_ID;
            break;
        }
        son = GetNode(son->brother_);
    }
}

void TreeAPI::Pivot(ArcPtr &arc_in, ArcPtr &arc_out) {
    //Update Flow
    UpdateCycleFlow(arc_in, arc_out);
    if (arc_in->arc_id_ == arc_out->arc_id_) {
        arc_out->status_ = (arc_out->flow_ == 0) ? FLOW_LOWER : FLOW_UPPER;
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
        auto arc_in = FindArcIn();
        if (arc_in == nullptr) {
            printf("Problem solved.\nIteration=%d Cost=%f\n", iter, GetTotalCost());
            break;
        }
        auto arc_out = FindArcOut(arc_in);
        if (arc_out == nullptr) {
            printf("Problem unbound.\nIteration=%d Cost=%f\n", iter, GetTotalCost());
            break;
        }
        if (debug_) {
            printf("Iteration %d\n", iter);
            PrintTreeAndBasis(*this);
            printf("Cycle\n");
            std::cout << "ArcIn: " << *arc_in
                      << " reduced: " << CalcReducedCost(arc_in)
                      << " volume: " << GetFlowCapacity(arc_in)
                      << std::endl;
            std::cout << "ArcOut: " << *arc_out
                      << " reduced: " << CalcReducedCost(arc_out)
                      << " volume: " << GetFlowCapacity(arc_out)
                      << std::endl;
            PrintNode(GetNode(cycle_.node_joint));
        }
        Pivot(arc_in, arc_out);
        if (iter >= max_iter) {
            printf("Problem failed.\nIteration=%d Cost=%f\n", iter, GetTotalCost());
            PrintArc();
            PrintArc(arc_in);
            PrintArc(arc_out);
            break;
        }
        iter += 1;
    } while (true);
    UpdateReducedCost();
    return RET_SUCCESS;
}

}
