#ifndef VLSI_FINAL_PROJECT_PIN_HPP_
#define VLSI_FINAL_PROJECT_PIN_HPP_

#include <string>
#include "Rectangle.hpp"

class Pin {
public:
    Pin() = default;

    Pin(int _layer, int lf_x, int lf_y, int ur_x, int ur_y) {
        layer = _layer;
        rect = Rectangle(lf_x, lf_y, ur_x, ur_y);
    }

    std::string toString()
    {
    	std::string s;
    	s = rect.toString() + " , " + std::to_string(layer);
    	return s;
    }

    bool operator==(const Pin& that) const {
        return layer == that.layer && rect == that.rect;
    }

    bool operator!=(const Pin& that) const {
        return !(*this == that);
    }

public:
    int layer;
    Rectangle rect;
};
#endif
