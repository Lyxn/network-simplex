//
// Created by LyxnO on 2019/11/16.
//

#include "tree.h"

#include <array>
#include <stack>

#include "utils.h"

namespace network {

using std::stack;

int TreeAPI::InitBasisTree(const BasisArc &basis_arc, int root) {
    basis_arc_.clear();
    for (auto &key: basis_arc) {
        auto src = key.first;
        auto dst = key.second;
        auto p_arc = GetArc(src, dst);
        basis_arc_.insert(key);
        p_arc->SetStatus(FLOW_TREE);
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
    CalcReducedCost();
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
            p_child->pred_ = pred_id;
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

inline ArcPtr TreeAPI::GetBasisArc(int src, int dst) {
    auto p_arc = GetArc(src, dst);
    if (p_arc != nullptr && p_arc->IsStatusTree()) {
        return p_arc;
    } else {
        return GetArc(dst, src);
    }
}

void TreeAPI::CalcNodePrice(NodePtr cur, NodePtr unk) {
    auto p_arc = GetBasisArc(cur->node_id_, unk->node_id_);
    if (p_arc->GetSrcId() == cur->node_id_) {
        unk->price_ = cur->price_ - p_arc->cost_;
    } else {
        unk->price_ = cur->price_ + p_arc->cost_;
    }
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
            auto p_father = GetNode(p_brother->pred_);
            CalcNodePrice(p_father, p_brother);
            stk.push(p_brother->node_id_);
        }
        auto p_son = GetNode(p_cur->son_);
        if (p_son != nullptr) {
            CalcNodePrice(p_cur, p_son);
            stk.push(p_son->node_id_);
        }
    }
}

bool TreeAPI::IsBasisArc(const ArcKey &key) const {
    return HasValue(basis_arc_, key);
}

void TreeAPI::CalcReducedCost() {
    for (auto &item: arc_idx_) {
        auto key = item.first;
        auto p_arc = item.second;
        if (IsBasisArc(key)) {
            p_arc->reduced_cost_ = 0;
        } else {
            auto p_src = GetNode(p_arc->GetSrcId());
            auto p_dst = GetNode(p_arc->GetDstId());
            p_arc->reduced_cost_ = p_arc->cost_ - (p_src->price_ - p_dst->price_);
        }
    }
}

double TreeAPI::GetTotalCost() const {
    double cost = 0;
    for (auto &key: basis_arc_) {
        auto p_arc = GetArc(key);
        cost += p_arc->cost_ * p_arc->flow_;
    }
    return cost;
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

void PrintBasisArc(const TreeAPI &tree) {
    for (auto &key: tree.basis_arc_) {
        auto p_arc = tree.GetArc(key);
        printf("%d %d %d %0.1f %0.1f\n",
               p_arc->src_id_,
               p_arc->dst_id_,
               p_arc->flow_,
               p_arc->cost_,
               p_arc->reduced_cost_
        );
    }
}

void PrintNonBasisArc(const TreeAPI &tree) {
    for (auto &it: tree.arc_idx_) {
        if (tree.IsBasisArc(it.first)) {
            continue;
        }
        auto p_arc = it.second;
        printf("%d %d %d %0.1f %0.1f\n",
               p_arc->src_id_,
               p_arc->dst_id_,
               p_arc->flow_,
               p_arc->cost_,
               p_arc->reduced_cost_
        );
    }
}

}
