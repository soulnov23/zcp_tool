#include "graph.h"

edge_node::edge_node() {}

edge_node::~edge_node() {}

vertex_node::vertex_node() {}

vertex_node::~vertex_node() {
    set<edge_node*>::const_iterator it;
    for (it = edge_node_set.begin(); it != edge_node_set.end(); it++) {
        delete *it;
    }
}

void vertex_node::add_edge(edge_node* node) {
    set<edge_node*>::const_iterator it;
    for (it = edge_node_set.begin(); it != edge_node_set.end(); it++) {
        if (**it == *node) {
            delete node;
            return;
        }
    }
    edge_node_set.emplace(node);
}

edge_node* vertex_node::add_edge(const string& name, const int& weight) {
    edge_node* node = new edge_node(name, weight);
    set<edge_node*>::const_iterator it;
    for (it = edge_node_set.begin(); it != edge_node_set.end(); it++) {
        if (**it == *node) {
            delete node;
            return *it;
        }
    }
    edge_node_set.emplace(node);
    return node;
}

void vertex_node::delete_edge(edge_node* node) {
    delete node;
    edge_node_set.erase(node);
}

void vertex_node::delete_edge(const string& name) {
    set<edge_node*>::const_iterator it;
    for (it = edge_node_set.begin(); it != edge_node_set.end(); it++) {
        if ((*it)->name == name) {
            delete *it;
            edge_node_set.erase(it);
            return;
        }
    }
}

graph_adjvex_list::graph_adjvex_list() {}

graph_adjvex_list::~graph_adjvex_list() {
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        delete *it;
    }
}

void graph_adjvex_list::add_vertex(vertex_node* node) {
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        if (**it == *node) {
            delete node;
            return;
        }
    }
    vertex_node_set.emplace(node);
}

vertex_node* graph_adjvex_list::add_vertex(const string& name) {
    vertex_node* node = new vertex_node(name);
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        if (**it == *node) {
            delete node;
            return *it;
        }
    }
    vertex_node_set.emplace(node);
    return node;
}

void graph_adjvex_list::delete_vertex(vertex_node* node) {
    delete node;
    vertex_node_set.erase(node);
}

void graph_adjvex_list::delete_vertex(const string& name) {
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        if ((*it)->name == name) {
            delete *it;
            vertex_node_set.erase(it);
            return;
        }
    }
}

bool graph_adjvex_list::find_edge(const string& src_name, const string& dst_name) {
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        if ((*it)->name == src_name) {
            set<edge_node*>& edge_node_set = (*it)->get_node_set();
            set<edge_node*>::const_iterator it_i;
            for (it_i = edge_node_set.begin(); it_i != edge_node_set.end(); it_i++) {
                if ((*it_i)->name == dst_name) {
                    return true;
                }
            }
        }
    }
    return false;
}

string graph_adjvex_list::dump_dot() {
    string content;
    content.append("digraph {\n");
    set<vertex_node*>::const_iterator it;
    for (it = vertex_node_set.begin(); it != vertex_node_set.end(); it++) {
        set<edge_node*>& edge_node_set = (*it)->get_node_set();
        set<edge_node*>::const_iterator it_i;
        for (it_i = edge_node_set.begin(); it_i != edge_node_set.end(); it_i++) {
            content.append("\t");
            content += (*it)->name + " -> " + (*it_i)->name + " [weight = " + to_string((*it_i)->weight) + "];\n";
        }
    }
    content.append("}\n");
    return content;
}
