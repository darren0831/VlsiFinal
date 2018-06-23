#ifndef VLSI_FINAL_PROJECT_PIN_HPP_
#define VLSI_FINAL_PROJECT_PIN_HPP_

#include <string>
#include "Rectangle.hpp"

class Pin {
public:
    Pin() = default;

    Pin(int layer, int llx, int lly, int urx, int ury) :
        layer(layer), rect(Rectangle(llx, lly, urx, ury)) {}

    Pin(const Pin& that) {
        layer = that.layer;
        rect = that.rect;
    }

    Pin(Pin&& that) noexcept {
        layer = that.layer;
        rect = std::move(that.rect);
    }

    Pin& operator=(const Pin& that) {
        if (this != &that) {
            layer = that.layer;
            rect = that.rect;
        }
        return *this;
    }

    Pin& operator=(Pin&& that) noexcept {
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

#endif // VLSI_FINAL_PROJECT_PIN_HPP_
