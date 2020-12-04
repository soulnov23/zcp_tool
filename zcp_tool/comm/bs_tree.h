#ifndef __BS_TREE_H__
#define __BS_TREE_H__

#include <set>
#include <string>
using namespace std;

class node_t {
public:
    node_t() {
        value_       = 0;
        parent_      = nullptr;
        left_child_  = nullptr;
        right_child_ = nullptr;
    };
    node_t(int value, node_t* parent = nullptr, node_t* left_child = nullptr, node_t* right_child = nullptr) {
        value_       = value;
        parent_      = parent;
        left_child_  = left_child;
        right_child_ = right_child;
    }
    ~node_t(){};

    node_t* parent() { return parent_; }
    void set_parent(node_t* parent) { parent_ = parent; }

    node_t* left_child() { return left_child_; }
    void set_left_child(node_t* left_child) { left_child_ = left_child; }

    node_t* right_child() { return right_child_; }
    void set_right_child(node_t* right_child) { right_child_ = right_child; }

    int value() { return value_; }
    void set_value(int value) { value_ = value; }

private:
    int value_;
    node_t* parent_;
    node_t* left_child_;
    node_t* right_child_;
};

// Binary Search Tree
class bs_tree_t {
public:
    bs_tree_t();
    ~bs_tree_t();

    node_t* add_node(int value);
    void delete_node(int value);
    void delete_node(node_t* node);
    node_t* find_node(int value);

private:
    node_t* root_;
    set<node_t*> node_set_;
};

#endif