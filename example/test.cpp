#include "graph.h"
#include "printf_utils.h"

int main(int argc, char* argv[]) {
    graph_adjvex_list list;
    vertex_node* a_node = list.add_vertex("A");
    vertex_node* b_node = list.add_vertex("B");
    vertex_node* c_node = list.add_vertex("C");
    vertex_node* d_node = list.add_vertex("D");
    vertex_node* e_node = list.add_vertex("E");
    vertex_node* f_node = list.add_vertex("F");
    vertex_node* g_node = list.add_vertex("G");
    vertex_node* h_node = list.add_vertex("H");

    a_node->add_edge("B", 3);
    b_node->add_edge("C", 5);
    b_node->add_edge("D", 5);
    c_node->add_edge("F", 10);
    e_node->add_edge("B", 7);
    e_node->add_edge("F", 7);
    f_node->add_edge("G", 2);
    h_node->add_edge("D", 6);
    h_node->add_edge("F", 6);

    string data = list.dump_dot();
    PRINTF_DEBUG("%s", data.c_str());
    return 0;
}