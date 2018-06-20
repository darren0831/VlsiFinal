#ifndef VLSI_FINAL_PROJECT_OBSTACLE_HPP_
#define VLSI_FINAL_PROJECT_OBSTACLE_HPP_

#include <string>
#include "Rectangle.hpp"

class Obstacle {
public:
    Obstacle() = default;

    Obstacle(int _layer, int lf_x, int lf_y, int ur_x, int ur_y) {
        rect = Rectangle(lf_x, lf_y,ur_x,ur_y);
        layer = _layer;
    }

    std::string toString()
    {
    	std::string s;
    	s = rect.toString()+ " , " + std::to_string(layer);
    	return s;
    }

    bool operator==(const Obstacle& that) const {
        return layer == that.layer && rect == that.rect;
    }

    bool operator!=(const Obstacle& that) const {
        return !(*this == that);
    }

public:
    Rectangle rect;
    int layer;
};

#endif
