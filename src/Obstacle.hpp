#ifndef VLSI_FINAL_PROJECT_OBSTACLE_HPP_
#define VLSI_FINAL_PROJECT_OBSTACLE_HPP_
#include "Rectangle.hpp"
#include <string>
class Obstacle {
public:
    Obstacle() {}

    Obstacle(int _layer, int lf_x, int lf_y, int ur_x, int ur_y) {
        area = Rectangle(lf_x, lf_y,ur_x,ur_y);
        layer = _layer;
    }

    std::string print()
    {
    	std::string s;
    	s = area.print()+ " , " + std::to_string(layer);
    	return s;
    }

    bool operator==(const Obstacle& that) const {
        return layer == that.layer && area == that.area;
    }

    bool operator!=(const Obstacle& that) const {
        return !(*this == that);
    }

public:
    Rectangle area;
    int layer;
};

#endif
