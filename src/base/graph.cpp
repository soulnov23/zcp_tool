#include "src/base/graph.h"

#include <queue>
#include <stack>
using namespace std;
#include "src/base/printf_utils.h"

graph_adjvex_list_t::graph_adjvex_list_t() {}

graph_adjvex_list_t::~graph_adjvex_list_t() {
    for (auto it : vertex_set) {
        delete it;
    }
    for (auto it : edge_set) {
        delete it;
    }
}

vertex_t* graph_adjvex_list_t::add_vertex(const string& name) {
    for (auto it : vertex_set) {
        if (it->name() == name) {
            return it;
        }
    }
    vertex_t* vertex = new vertex_t(name);
    if (vertex == nullptr) {
        return nullptr;
    }
    vertex_set.insert(vertex);
    return vertex;
}

void graph_adjvex_list_t::delete_vertex(const string& name) {
    vertex_t* vertex = find_vertex(name);
    delete_vertex(vertex);
}

void graph_adjvex_list_t::delete_vertex(vertex_t* vertex) {
    if (vertex == nullptr) {
        return;
    }
    auto it = vertex_set.find(vertex);
    if (it == vertex_set.end()) {
        return;
    }
    auto in_edge_set = vertex->in_edge_set();
    // 在入度表里找到其它顶点，删除其它顶点的出表信息
    // edge的析构函数会释放自己的入度表
    for (auto it : in_edge_set) {
        it->src()->delete_out_edge(it);
        edge_set.erase(it);
        delete it;
    }
    auto out_edge_set = vertex->out_edge_set();
    // 在出度表里找到其它顶点，删除其它顶点的入表信息
    // edge的析构函数会释放自己的出度表
    for (auto it : out_edge_set) {
        it->dst()->delete_in_edge(it);
        edge_set.erase(it);
        delete it;
    }
    vertex_set.erase(vertex);
    delete vertex;
}

edge_t* graph_adjvex_list_t::add_edge(vertex_t* src_vertex, vertex_t* dst_vertex, const string& weight) {
    edge_t* edge = new edge_t(src_vertex, dst_vertex, weight);
    if (edge == nullptr) {
        return nullptr;
    }
    src_vertex->add_out_edge(edge);
    dst_vertex->add_in_edge(edge);
    edge_set.insert(edge);
    return edge;
}

void graph_adjvex_list_t::delete_edge(vertex_t* src_vertex, vertex_t* dst_vertex) {
    if (src_vertex == nullptr || dst_vertex == nullptr) {
        return;
    }
    for (auto it : edge_set) {
        if (it->src() == src_vertex && it->dst() == dst_vertex) {
            src_vertex->delete_out_edge(it);
            dst_vertex->delete_in_edge(it);
            edge_set.erase(it);
            delete it;
        }
    }
}

void graph_adjvex_list_t::delete_edge(edge_t* edge) {
    if (edge == nullptr) {
        return;
    }
    auto it = edge_set.find(edge);
    if (it == edge_set.end()) {
        return;
    }
    edge->src()->delete_out_edge(edge);
    edge->dst()->delete_in_edge(edge);
    edge_set.erase(edge);
    delete edge;
}

vertex_t* graph_adjvex_list_t::find_vertex(const string& vertex_name) {
    for (auto it : vertex_set) {
        if (it->name() == vertex_name) {
            return it;
        }
    }
    return nullptr;
}

bool graph_adjvex_list_t::find_edge(const string& src_name, const string& dst_name) {
    vertex_t* src_vertex = nullptr;
    vertex_t* dst_vertex = nullptr;
    for (auto it : vertex_set) {
        if (it->name() == src_name) {
            src_vertex = it;
        }
        if (it->name() == dst_name) {
            dst_vertex = it;
        }
    }
    if ((src_vertex == nullptr) || (dst_vertex == nullptr)) {
        return false;
    }
    for (auto it : edge_set) {
        if (it->src() == src_vertex && it->dst() == dst_vertex) {
            return true;
        }
    }
    return false;
}

string graph_adjvex_list_t::dump_dot() {
    string content;
    content.append("\ndigraph {\n");
    set<vertex_t*>::const_iterator it;
    for (auto it : vertex_set) {
        for (auto it_i : it->out_edge_set()) {
            content.append("\t");
            content += it_i->src()->name() + " -> " + it_i->dst()->name() + " [weight = " + it_i->weight() + "];\n";
        }
    }
    content.append("}\n");
    return content;
}

string graph_adjvex_list_t::bfs_traverse() {
    string content;
    map<vertex_t*, bool> visited;
    for (auto it : vertex_set) {
        visited[it] = false;
    }
    queue<vertex_t*> vertex_queue;
    // 顶点挨个入栈
    for (auto it : vertex_set) {
        if (!visited[it]) {
            vertex_queue.push(it);
            visited[it] = true;
            content += it->name() + " -> ";
        }
        // 查找顶点的所有未访问的邻接点
        while (!vertex_queue.empty()) {
            vertex_t* vertex = vertex_queue.front();
            auto edge_set    = vertex->out_edge_set();
            auto it_i        = edge_set.begin();
            for (; it_i != edge_set.end(); it_i++) {
                if (!visited[(*it_i)->dst()]) {
                    vertex_queue.push((*it_i)->dst());
                    visited[(*it_i)->dst()] = true;
                    content += (*it_i)->dst()->name() + " -> ";
                }
            }
            // 子邻接点没有了，pop顶点回溯到上一层
            if (it_i == edge_set.end()) {
                vertex_queue.pop();
            }
        }
    }
    return content.substr(0, content.length() - 4);
}

string graph_adjvex_list_t::dfs_traverse() {
    string content;
    map<vertex_t*, bool> visited;
    for (auto it : vertex_set) {
        visited[it] = false;
    }
    stack<vertex_t*> vertex_stack;
    // 顶点挨个入栈
    for (auto it : vertex_set) {
        if (!visited[it]) {
            vertex_stack.push(it);
            visited[it] = true;
            content += it->name() + " -> ";
        }
        // 查找顶点的第一个未访问的邻接点，然后break出来继续访问子邻接点
        while (!vertex_stack.empty()) {
            vertex_t* vertex = vertex_stack.top();
            auto edge_set    = vertex->out_edge_set();
            auto it_i        = edge_set.begin();
            for (; it_i != edge_set.end(); it_i++) {
                if (!visited[(*it_i)->dst()]) {
                    vertex_stack.push((*it_i)->dst());
                    visited[(*it_i)->dst()] = true;
                    content += (*it_i)->dst()->name() + " -> ";
                    break;
                }
            }
            // 子邻接点没有了，pop顶点回溯到上一层
            if (it_i == edge_set.end()) {
                vertex_stack.pop();
            }
        }
    }
    return content.substr(0, content.length() - 4);
}

void graph_adjvex_list_t::dfs_traverse_recursion_(vertex_t* vertex, map<vertex_t*, bool>& visited, string& content) {
    if (visited[vertex]) {
        return;
    }
    visited[vertex] = true;
    content += vertex->name() + " -> ";
    auto edge_set = vertex->out_edge_set();
    for (auto it : edge_set) {
        dfs_traverse_recursion_(it->dst(), visited, content);
    }
}

string graph_adjvex_list_t::dfs_traverse_recursion() {
    string content;
    map<vertex_t*, bool> visited;
    for (auto it : vertex_set) {
        visited[it] = false;
    }
    for (auto it : vertex_set) {
        dfs_traverse_recursion_(it, visited, content);
    }
    return content.substr(0, content.length() - 4);
}

// DAG可能存在多个拓扑序列
string graph_adjvex_list_t::topological_sort() {
    string content;
    queue<vertex_t*> vertex_queue;
    map<vertex_t*, size_t> in_degree_map;
    for (auto it : vertex_set) {
        in_degree_map[it] = it->in_degree();
        if (it->in_degree() == 0) {
            vertex_queue.push(it);
        }
    }
    size_t count = 0;
    while (!vertex_queue.empty()) {
        vertex_t* vertex = vertex_queue.front();
        auto edge_set    = vertex->out_edge_set();
        for (auto it_i : edge_set) {
            in_degree_map[it_i->dst()] = in_degree_map[it_i->dst()] - 1;
            if (in_degree_map[it_i->dst()] == 0) {
                vertex_queue.push(it_i->dst());
            }
        }
        vertex_queue.pop();
        count++;
        content += vertex->name() + " -> ";
    }
    if (count < vertex_count()) {
        PRINTF_ERROR("DAG have cycle");
        return "";
    }
    return content.substr(0, content.length() - 4);
}

string graph_adjvex_list_t::critical_path() {}