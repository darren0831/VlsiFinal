#ifndef VLSI_FINAL_PROJECT_TRACK_HPP_
#define VLSI_FINAL_PROJECT_TRACK_HPP_

#include <string>
#include "Rectangle.hpp"

class Track {
public:
    Track(double llx, double lly, double urx, double ury, double w, int l) {
        terminal[0] = Point(llx, lly);
        terminal[1] = Point(urx, ury);

        if (llx == urx) {
            rect = Rectangle(llx - w / 2, lly, urx + w / 2, ury);
        } else if (lly == ury) {
            rect = Rectangle(llx, lly - w / 2, urx, ury + w / 2);
        }

        width = w;
        layer = l;
    }

    Rectangle overlap(const Track& that, bool lineOverlap) const {
        return rect.overlap(that.rect, lineOverlap);
    }

    bool hasOverlap(const Track& that, bool lineOverlap) const {
        return rect.hasOverlap(that.rect, lineOverlap);
    }

    std::string toString() const {
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

#endif // VLSI_FINAL_PROJECT_TRACK_HPP_
