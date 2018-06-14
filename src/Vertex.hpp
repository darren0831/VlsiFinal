#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

class Vertex {
public:
    Vertex() {}
    Vertex(int _id, int _x, int _y, int _layer) : x(_x), y(_y), layer(_layer), id(_id) {}
    int x, y;
    int layer;
    int id;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_HPP_
