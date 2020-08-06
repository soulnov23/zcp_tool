#include "graph.h"
#include "printf_utils.h"

int main(int argc, char* argv[]) {
    vertex_t* A = new vertex_t("A");
    vertex_t* B = new vertex_t("B");
    vertex_t* C = new vertex_t("C");
    vertex_t* D = new vertex_t("D");
    vertex_t* E = new vertex_t("E");
    vertex_t* F = new vertex_t("F");
    vertex_t* G = new vertex_t("G");
    vertex_t* H = new vertex_t("H");

    A->add_out_edge(B, "3");
    B->add_out_edge(C, "5");
    B->add_out_edge(D, "5");
    C->add_out_edge(F, "10");
    E->add_out_edge(B, "7");
    E->add_out_edge(F, "7");
    F->add_out_edge(G, "2");
    H->add_out_edge(D, "6");
    H->add_out_edge(F, "6");

    graph_adjvex_list_t list;
    list.add_vertex(A);
    list.add_vertex(B);
    list.add_vertex(C);
    list.add_vertex(D);
    list.add_vertex(E);
    list.add_vertex(F);
    list.add_vertex(G);
    list.add_vertex(H);

    string data = list.dump_dot();
    PRINTF_DEBUG("%s", data.c_str());

    if (list.find_edge("A", "B")) {
        PRINTF_DEBUG("AB");
    }
    if (list.find_edge("B", "C")) {
        PRINTF_DEBUG("BC");
    }
    if (list.find_edge("A", "C")) {
        PRINTF_DEBUG("AC");
    }

    list.dfs_traverse_recursion();
    list.dfs_traverse();
    list.bfs_traverse();
    return 0;
}