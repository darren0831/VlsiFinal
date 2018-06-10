#ifndef VLSI_FIANL_PROJECT_TRACK_HPP_
#define VLSI_FIANL_PROJECT_TRACK_HPP_
#include "Rectangle.hpp"
#include <string>
class Track {
public:
    Track(double _lx, double _ly, double _rx, double _ry, double _w, int _layer) {
        if( _lx == _rx)
        {
        	rect = Rectangle(_lx - _w / 2, _ly, _rx + _w / 2, _ry);
        }else if ( _ly == _ry)
        	rect = Rectangle(_lx, _ly - _w / 2, _rx, _ry + _w / 2);
        width = _w;
        layer = _layer;
    }
    Rectangle rect;
    double width;
    int layer;

    std::string print()
    {
    	std::string s;
    	s = rect.print() + " , " + std::to_string(width) + " , " + std::to_string(layer);
    	return s;
    }
};

#endif
