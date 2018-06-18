#ifndef VLSI_FINAL_PROJECT_TRACK_HPP_
#define VLSI_FINAL_PROJECT_TRACK_HPP_
#include "Rectangle.hpp"
#include <string>
class Track {
public:
    Track(double _lx, double _ly, double _rx, double _ry, double _w, int _layer) {
        terminal[0] = Point(_lx, _ly);
        terminal[1] = Point(_rx, _ry);

        if (_lx == _rx) {
            rect = Rectangle(_lx - _w / 2, _ly, _rx + _w / 2, _ry);
        } else if (_ly == _ry)
            rect = Rectangle(_lx, _ly - _w / 2, _rx, _ry + _w / 2);

        width = _w;
        layer = _layer;
    }

    std::string print() {
        std::string s;
        s = rect.print() + " , " + std::to_string(width) + " , " + std::to_string(layer);
        return s;
    }

public:
    Point terminal[2];
    Rectangle rect;
    double width;
    int layer;
};

#endif
