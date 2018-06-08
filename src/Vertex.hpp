#ifndef VLSI_FIANL_PROJECT_VERTEX_HPP_
#define VLSI_FIANL_PROJECT_VERTEX_HPP_

class Vertex
{
public:
    Vertex(){}
    Vertex(int _id, int _x, int _y, int _layer) :x(_x),y(_y),layer(_layer),id(_id){}
    int x, y;
    int layer;
    int id;
    int connect_id;
    void setConnenctId(int _id)
    {
        connect_id=_id;
    }

};

#endif