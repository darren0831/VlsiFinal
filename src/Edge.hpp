#ifndef VLSI_FINAL_PROJECT_EDGE_HPP_
#define VLSI_FINAL_PROJECT_EDGE_HPP_

class Edge {
public:
    Edge() {}

    Edge(int src, int tgt) :
        src(src), tgt(tgt) {}

public:
    int src;
    int tgt;
    int trackWidth;
};

#endif // VLSI_FINAL_PROJECT_EDGE_HPP_
