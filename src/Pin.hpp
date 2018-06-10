#ifndef VLSI_FIANL_PROJECT_PIN_HPP_
#define VLSI_FIANL_PROJECT_PIN_HPP_
#include "Rectangle.hpp"
#include <string>
class Pin {
public:
    Pin() {}
    Pin(int _layer, int lf_x, int lf_y, int ur_x, int ur_y) {
        layer = _layer;
        r = Rectangle(lf_x, lf_y, ur_x, ur_y);
    }
    int layer;
    Rectangle r;
    std::string print()
    {
    	std::string s;
    	s = r.print() + " , " + std::to_string(layer);
    	return s;
    }

};
#endif