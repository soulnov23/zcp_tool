#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <map>
#include <set>
#include <string>
using namespace std;

class edge_t;
class vertex_t;

// 边
class edge_t {
public:
    edge_t(){};
    edge_t(vertex_t* s, vertex_t* d, const string& w) {
        src_    = s;
        dst_    = d;
        weight_ = w;
    }
    ~edge_t(){};
    bool operator==(edge_t* edge) {
        if ((edge->src_ == src_) && (edge->dst_ == dst_)) {
            return true;
        }
        return false;
    }
    bool operator==(edge_t edge) {
        if ((edge.src_ == src_) && (edge.dst_ == dst_)) {
            return true;
        }
        return false;
    }

    string weight() { return weight_; }
    vertex_t* src() { return src_; }
    vertex_t* dst() { return dst_; }

private:
    vertex_t* src_;
    vertex_t* dst_;
    string weight_;
};

// 顶点
class vertex_t {
public:
    vertex_t(){};
    vertex_t(const string& n) { name_ = n; }
    ~vertex_t() {
        for (auto it : edge_set_) {
            delete it;
        }
    }
    bool operator==(vertex_t* vertex) {
        if (vertex->name_ == name_) {
            return true;
        }
        return false;
    }
    bool operator==(vertex_t& vertex) {
        if (vertex.name_ == name_) {
            return true;
        }
        return false;
    }
    // 入度
    void add_in_edge(vertex_t* vertex, string& weight) {
        edge_t* edge = new edge_t(vertex, this, weight);
        for (auto it : edge_set_) {
            if (*it == *edge) {
                delete edge;
                return;
            }
        }
        edge_set_.insert(edge);
    }
    void delete_in_edge(vertex_t* vertex) {
        for (auto it : edge_set_) {
            if (it->src() == vertex) {
                edge_set_.erase(it);
                delete it;
                return;
            }
        }
    }
    // 出度
    void add_out_edge(vertex_t* vertex, const string& weight) {
        edge_t* edge = new edge_t(this, vertex, weight);
        for (auto it : edge_set_) {
            if (*it == *edge) {
                delete edge;
                return;
            }
        }
        edge_set_.insert(edge);
    }
    void delete_out_edge(vertex_t* vertex) {
        for (auto it : edge_set_) {
            if (it->dst() == vertex) {
                edge_set_.erase(it);
                delete it;
                return;
            }
        }
    }
    bool find_in_edge(vertex_t* vertex) {
        for (auto it : edge_set_) {
            if (it->src() == vertex) {
                return true;
            }
        }
        return false;
    }
    bool find_out_edge(vertex_t* vertex) {
        for (auto it : edge_set_) {
            if (it->dst() == vertex) {
                return true;
            }
        }
        return false;
    }
    string name() { return name_; }
    set<edge_t*>& edge_set() { return edge_set_; }

private:
    set<edge_t*> edge_set_;
    string name_;
};
// 邻接链表
class graph_adjvex_list_t {
public:
    graph_adjvex_list_t();
    ~graph_adjvex_list_t();

    void add_vertex(vertex_t* vertex);
    void delete_vertex(vertex_t* vertex);

    vertex_t* find_vertex(const string& vertex_name);
    bool find_edge(const string& src_name, const string& dst_name);

    string dump_dot();
    void dfs_traverse();
    void bfs_traverse();
    void dfs_traverse_recursion();

private:
    void dfs_traverse_recursion_(vertex_t* vertex, map<vertex_t*, bool>& visited);

private:
    set<vertex_t*> vertex_set;
};

// 邻接矩阵
class graph_adjvex_array {
public:
    graph_adjvex_array();
    ~graph_adjvex_array();

private:
    int vertex_count;
    int edge_count;
    set<vertex_t*> vertex_set;   // 顶点表
    map<edge_t*, int> edge_map;  // 顶点关系表 int(0, 1, 无穷大)
};

#endif