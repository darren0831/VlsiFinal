#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

class Vertex {
public:
    Vertex() {}

    Vertex(int _id, double _lx, double _ly, double _rx, double _ry, int _layer) : layer(_layer), id(_id)
    {
    	coordinate = Rectangle(_lx,_ly,_rx,_ry);
    }

public:
    Rectangle coordinate;
    int layer;
    int id;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_HPP_
