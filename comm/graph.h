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
    edge_t(vertex_t* src, vertex_t* dst, const string& weight) {
        src_    = src;
        dst_    = dst;
        weight_ = weight;
    }
    ~edge_t(){};
    vertex_t* src() { return src_; }
    vertex_t* dst() { return dst_; }
    string weight() { return weight_; }
    void set_weight(const string& weight) { weight_ = weight; }

private:
    vertex_t* src_;
    vertex_t* dst_;
    string weight_;
};

// 顶点
class vertex_t {
public:
    vertex_t(){};
    vertex_t(const string& name) { name_ = name; }
    ~vertex_t() {}
    // 入度
    void add_in_edge(edge_t* edge) { in_edge_set_.insert(edge); }
    void delete_in_edge(edge_t* edge) {
        in_edge_set_.erase(edge);
        delete edge;
    }
    // 出度
    void add_out_edge(edge_t* edge) { out_edge_set_.insert(edge); }
    void delete_out_edge(edge_t* edge) {
        out_edge_set_.erase(edge);
        delete edge;
    }

    set<edge_t*>& in_edge_set() { return in_edge_set_; }
    set<edge_t*>& out_edge_set() { return out_edge_set_; }
    string name() { return name_; }
    void set_name(const string& name) { name_ = name; }

    size_t in_degree() { return in_edge_set_.size(); }
    size_t out_degree() { return out_edge_set_.size(); }

private:
    set<edge_t*> in_edge_set_;
    set<edge_t*> out_edge_set_;
    string name_;
};
// 邻接链表
class graph_adjvex_list_t {
public:
    graph_adjvex_list_t();
    ~graph_adjvex_list_t();

    vertex_t* add_vertex(const string& name);
    void delete_vertex(const string& name);
    void delete_vertex(vertex_t* vertex);

    edge_t* add_edge(vertex_t* src_vertex, vertex_t* dst_vertex, const string& weight);
    void delete_edge(vertex_t* src_vertex, vertex_t* dst_vertex);
    void delete_edge(edge_t* edge);

    vertex_t* find_vertex(const string& vertex_name);
    bool find_edge(const string& src_name, const string& dst_name);

    string dump_dot();

    string dfs_traverse();
    string bfs_traverse();
    string dfs_traverse_recursion();

    string topological_sort();
    string critical_path();

    size_t vertex_count() { return vertex_set.size(); }
    size_t edge_count() { return edge_set.size(); }

private:
    void dfs_traverse_recursion_(vertex_t* vertex, map<vertex_t*, bool>& visited, string& content);

private:
    set<vertex_t*> vertex_set;
    set<edge_t*> edge_set;
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