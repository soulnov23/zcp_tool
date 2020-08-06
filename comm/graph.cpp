#include "graph.h"
#include <queue>
#include <stack>
using namespace std;
#include "printf_utils.h"

graph_adjvex_list_t::graph_adjvex_list_t() {}

graph_adjvex_list_t::~graph_adjvex_list_t() {
    for (auto it : vertex_set) {
        delete it;
    }
}

void graph_adjvex_list_t::add_vertex(vertex_t* vertex) {
    for (auto it : vertex_set) {
        if (*it == *vertex) {
            delete vertex;
            return;
        }
    }
    vertex_set.insert(vertex);
}

void graph_adjvex_list_t::delete_vertex(vertex_t* vertex) {
    delete vertex;
    vertex_set.erase(vertex);
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
    return src_vertex->find_out_edge(dst_vertex);
}

string graph_adjvex_list_t::dump_dot() {
    string content;
    content.append("\ndigraph {\n");
    set<vertex_t*>::const_iterator it;
    for (auto it : vertex_set) {
        for (auto it_i : it->edge_set()) {
            content.append("\t");
            content += it_i->src()->name() + " -> " + it_i->dst()->name() + " [weight = " + it_i->weight() + "];\n";
        }
    }
    content.append("}\n");
    return content;
}

void graph_adjvex_list_t::bfs_traverse() {
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
            PRINTF_DEBUG("%s", it->name().c_str());
        }
        // 查找顶点的所有未访问的邻接点
        while (!vertex_queue.empty()) {
            vertex_t* vertex = vertex_queue.front();
            auto edge_set    = vertex->edge_set();
            auto it_i        = edge_set.begin();
            for (; it_i != edge_set.end(); it_i++) {
                if (!visited[(*it_i)->dst()]) {
                    vertex_queue.push((*it_i)->dst());
                    visited[(*it_i)->dst()] = true;
                    PRINTF_DEBUG("%s", (*it_i)->dst()->name().c_str());
                }
            }
            // 子邻接点没有了，pop顶点回溯到上一层
            if (it_i == edge_set.end()) {
                vertex_queue.pop();
            }
        }
    }
}

void graph_adjvex_list_t::dfs_traverse() {
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
            PRINTF_DEBUG("%s", it->name().c_str());
        }
        // 查找顶点的第一个未访问的邻接点，然后break出来继续访问子邻接点
        while (!vertex_stack.empty()) {
            vertex_t* vertex = vertex_stack.top();
            auto edge_set    = vertex->edge_set();
            auto it_i        = edge_set.begin();
            for (; it_i != edge_set.end(); it_i++) {
                if (!visited[(*it_i)->dst()]) {
                    vertex_stack.push((*it_i)->dst());
                    visited[(*it_i)->dst()] = true;
                    PRINTF_DEBUG("%s", (*it_i)->dst()->name().c_str());
                    break;
                }
            }
            // 子邻接点没有了，pop顶点回溯到上一层
            if (it_i == edge_set.end()) {
                vertex_stack.pop();
            }
        }
    }
}

void graph_adjvex_list_t::dfs_traverse_recursion_(vertex_t* vertex, map<vertex_t*, bool>& visited) {
    if (visited[vertex]) {
        return;
    }
    visited[vertex] = true;
    PRINTF_DEBUG("%s", vertex->name().c_str());
    auto edge_set = vertex->edge_set();
    for (auto it : edge_set) {
        dfs_traverse_recursion_(it->dst(), visited);
    }
}

void graph_adjvex_list_t::dfs_traverse_recursion() {
    map<vertex_t*, bool> visited;
    for (auto it : vertex_set) {
        visited[it] = false;
    }
    for (auto it : vertex_set) {
        dfs_traverse_recursion_(it, visited);
    }
}
