#ifndef VLSI_FINAL_PROJECT_OBSTACLE_HPP_
#define VLSI_FINAL_PROJECT_OBSTACLE_HPP_

#include <string>
#include "Rectangle.hpp"

class Obstacle {
public:
    Obstacle() = default;

    Obstacle(int layer, int llx, int lly, int urx, int ury) :
        layer(layer), rect(Rectangle(llx, lly, urx, ury)) {}

    std::string toString() const {
        std::string s;
        s = rect.toString() + " , " + std::to_string(layer);
        return s;
    }

    bool operator==(const Obstacle& that) const {
        return layer == that.layer && rect == that.rect;
    }

    bool operator!=(const Obstacle& that) const {
        return !(*this == that);
    }

public:
    int layer;
    Rectangle rect;
};

#endif // VLSI_FINAL_PROJECT_OBSTACLE_HPP_
