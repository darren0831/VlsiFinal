#ifndef VLSI_FIANL_PROJECT_EDGE_HPP_
#define VLSI_FIANL_PROJECT_EDGE_HPP_

class Edge {
public:
    Edge() {}

    Edge(int src, int tgt) :
        src(src), tgt(tgt) {}

    int src;
    int tgt;
    int trackWidth;
};

#endif // VLSI_FIANL_PROJECT_EDGE_HPP_
