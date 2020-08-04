#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <set>
#include <string>
using namespace std;

// 顶点
class edge_node {
public:
    edge_node();
    edge_node(const string& n, const int& w) {
        name   = n;
        weight = w;
    }
    ~edge_node();
    bool operator==(edge_node* node) {
        if ((node->name == name) && (node->weight == weight)) {
            return true;
        }
        return false;
    }
    bool operator==(edge_node node) {
        if ((node.name == name) && (node.weight == weight)) {
            return true;
        }
        return false;
    }

public:
    string name;
    int weight;
};

// 边节点
class vertex_node {
public:
    vertex_node();
    vertex_node(const string& n) { name = n; }
    ~vertex_node();
    bool operator==(vertex_node* node) {
        if (node->name == name) {
            return true;
        }
        return false;
    }
    bool operator==(vertex_node node) {
        if (node.name == name) {
            return true;
        }
        return false;
    }

    void add_edge(edge_node* node);
    edge_node* add_edge(const string& name, const int& weight);
    void delete_edge(edge_node* node);
    void delete_edge(const string& name);

    set<edge_node*>& get_node_set() { return edge_node_set; }

public:
    string name;

private:
    set<edge_node*> edge_node_set;
};

// 邻接链表
class graph_adjvex_list {
public:
    graph_adjvex_list();
    ~graph_adjvex_list();

    void add_vertex(vertex_node* node);
    vertex_node* add_vertex(const string& name);
    void delete_vertex(vertex_node* node);
    void delete_vertex(const string& name);

    string dump_dot();

private:
    set<vertex_node*> vertex_node_set;
};

// 邻接矩阵
class graph_adjvex_array {
public:
    graph_adjvex_array();
    ~graph_adjvex_array();
};

#endif