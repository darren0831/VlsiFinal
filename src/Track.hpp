#ifndef VLSI_FINAL_PROJECT_TRACK_HPP_
#define VLSI_FINAL_PROJECT_TRACK_HPP_

#include <string>
#include "Rectangle.hpp"

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

    Rectangle overlap(const Track& that) const {
        return rect.overlap(that.rect);
    }

    bool hasOverlap(const Track& that) const {
        return rect.hasOverlap(that.rect);
    }

    std::string toString() {
        std::string s;
        s = rect.toString() + " , " + std::to_string(width) + " , " + std::to_string(layer);
        return s;
    }

    bool operator==(const Track& that) const {
        if (terminal[0] != that.terminal[0]) {
            return false;
        }
        if (terminal[1] != that.terminal[1]) {
            return false;
        }
        double dw = fabs(width - that.width);
        return rect == that.rect && dw < 1e-6 && layer == that.layer;
    }

    bool operator!=(const Track& that) const {
        return !(*this == that);
    }

public:
    Point terminal[2];
    Rectangle rect;
    double width;
    int layer;
};

#endif
