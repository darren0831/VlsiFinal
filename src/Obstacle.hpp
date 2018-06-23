#ifndef VLSI_FINAL_PROJECT_OBSTACLE_HPP_
#define VLSI_FINAL_PROJECT_OBSTACLE_HPP_

#include <string>
#include "Rectangle.hpp"

class Obstacle {
public:
    Obstacle() = default;

    Obstacle(int layer, int llx, int lly, int urx, int ury) :
        layer(layer), rect(Rectangle(llx, lly, urx, ury)) {}

    Obstacle(const Obstacle& that) {
        layer = that.layer;
        rect = that.rect;
    }

    Obstacle(Obstacle&& that) noexcept {
        layer = that.layer;
        rect = std::move(that.rect);
    }

    Obstacle& operator=(const Obstacle& that) {
        if (this != &that) {
            layer = that.layer;
            rect = that.rect;
        }
        return *this;
    }

    Obstacle& operator=(Obstacle&& that) noexcept {
        if (this != &that) {
            layer = that.layer;
            rect = std::move(that.rect);
        }
        return *this;
    }

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
