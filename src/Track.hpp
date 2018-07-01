#ifndef VLSI_FINAL_PROJECT_TRACK_HPP_
#define VLSI_FINAL_PROJECT_TRACK_HPP_

#include <string>
#include "Rectangle.hpp"

class Track {
public:
    Track() {
        terminal[0] = Point(-1, -1);
        terminal[1] = Point(-1, -1);
        rect = Rectangle(-1, -1, -1, -1);
        width = -1;
        layer = -1;
    }

    Track(double llx, double lly, double urx, double ury, double w, int l) {
        terminal[0] = Point(llx, lly);
        terminal[1] = Point(urx, ury);

        if (fabs(llx - urx) < 1e-6) {
            rect = Rectangle(llx - w / 2, lly, urx + w / 2, ury);
        } else if (fabs(lly - ury) < 1e-6) {
            rect = Rectangle(llx, lly - w / 2, urx, ury + w / 2);
        }

        width = w;
        layer = l;
    }

    Track(const Track& that) {
        terminal[0] = that.terminal[0];
        terminal[1] = that.terminal[1];
        rect = that.rect;
        width = that.width;
        layer = that.layer;
    }

    Track(Track&& that) noexcept {
        terminal[0] = std::move(that.terminal[0]);
        terminal[1] = std::move(that.terminal[1]);
        rect = std::move(that.rect);
        width = that.width;
        layer = that.layer;
    }

    Track& operator=(const Track& that) {
        if (this != &that) {
            terminal[0] = that.terminal[0];
            terminal[1] = that.terminal[1];
            rect = that.rect;
            width = that.width;
            layer = that.layer;
        }
        return *this;
    }

    Track& operator=(Track&& that) noexcept {
        if (this != &that) {
            terminal[0] = std::move(that.terminal[0]);
            terminal[1] = std::move(that.terminal[1]);
            rect = std::move(that.rect);
            width = that.width;
            layer = that.layer;
        }
        return *this;
    }

    Rectangle overlap(const Track& that, bool lineOverlap) const {
        return rect.overlap(that.rect, lineOverlap);
    }

    bool hasOverlap(const Track& that, bool lineOverlap) const {
        return rect.hasOverlap(that.rect, lineOverlap);
    }

    int length() const {
        if (fabs(terminal[1].x - terminal[0].x) < 1e-6) {
            return fabs(terminal[1].y - terminal[0].y);
        } else {
            return fabs(terminal[1].x - terminal[0].x);
        }
    }

    bool isHorizontal() const {
        return fabs(terminal[1].y - terminal[0].y) < 1e-6;
    }

    bool isVertical() const {
        return fabs(terminal[1].x - terminal[0].x) < 1e-6;
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
