#include "bs_tree.h"
#include "printf_utils.h"

bs_tree_t::bs_tree_t() { root_ = nullptr; }

bs_tree_t::~bs_tree_t() {
    if (root_ != nullptr) {
        delete root_;
        root_ = nullptr;
    }
}

node_t* bs_tree_t::add_node(int value) {
    if (root_ == nullptr) {
        root_ = new node_t(value, nullptr, nullptr, nullptr);
        return root_;
    }
    node_t* temp = root_;
    node_t* parent_temp;
    while (temp != nullptr) {
        parent_temp = temp;
        if (value == temp->value()) {
            return temp;
        } else if (value < temp->value()) {
            temp = temp->left_child();
        } else {  // value > temp->value()
            temp = temp->right_child();
        }
    }
    // 递归无法获取parent节点
    node_t* new_node = new node_t(value, parent_temp);
    if (value < parent_temp->value()) {
        parent_temp->set_left_child(new_node);
    } else {  // value > parent_temp->value()
        parent_temp->set_right_child(new_node);
    }
    return new_node;
}

void bs_tree_t::delete_node(int value) {
    node_t* node = find_node(value);
    delete_node(node);
}

void bs_tree_t::delete_node(node_t* node) {
    if (node == nullptr) {
        return;
    }
}

node_t* bs_tree_t::find_node(int value) {
    node_t* temp = root_;
    while (temp != nullptr) {
        if (value == temp->value()) {
            break;
        } else if (value < temp->value()) {
            temp = temp->left_child();
        } else {  // value > temp->value()
            temp = temp->right_child();
        }
    }
    return temp;
}