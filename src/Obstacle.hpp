#ifndef VLSI_FIANL_PROJECT_OBSTACLE_HPP_
#define VLSI_FIANL_PROJECT_OBSTACLE_HPP_
#include "Rectangle.hpp"
#include <string>
class Obstacle {
public:
    Obstacle() {}
    Obstacle(int _layer, int lf_x, int lf_y, int ur_x, int ur_y) {
        area = Rectangle(lf_x, lf_y,ur_x,ur_y);
        layer = _layer;
    }
    Rectangle area;
    std::string print()
    {
    	std::string s;
    	s = area.print()+ " , " + std::to_string(layer);
    	return s;
    }
};

#endif