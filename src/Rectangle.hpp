#ifndef VLSI_FINAL_PROJECT_RECTANGLE_HPP_
#define VLSI_FINAL_PROJECT_RECTANGLE_HPP_

#include <cmath>
#include <initializer_list>
#include <string>
#include "Point.hpp"

class Rectangle {
public:
    Rectangle() = default;

    Rectangle(double llx, double lly, double urx, double ury) {
        ll = Point(llx, lly);
        ur = Point(urx, ury);
        width = ur.x - ll.x;
        height = ur.y - ll.y;
    }

    Point midPoint() const {
        return ll.midPoint(ur);
    }

    Rectangle overlap(const Rectangle& that, bool const lineOverlap = false) const {
        Point lowerLeft = Point(0, 0), upperRight = Point(0, 0);

        if (lineOverlap) {
            if (ll.x <= that.ur.x && ur.x >= that.ll.x) {
                if (ll.y <= that.ur.y && ur.y >= that.ll.y) {
                    if (ll.x > that.ll.x) { lowerLeft.x = ll.x; }
                    else { lowerLeft.x = that.ll.x; }

                    if (ll.y > that.ll.y) { lowerLeft.y = ll.y; }
                    else { lowerLeft.y = that.ll.y; }

                    if (ur.x < that.ur.x) { upperRight.x = ur.x; }
                    else { upperRight.x = that.ur.x; }

                    if (ur.y < that.ur.y) { upperRight.y = ur.y; }
                    else { upperRight.y = that.ur.y; }
                }
            }
        } else {
            if (ll.x < that.ur.x && ur.x > that.ll.x) {
                if (ll.y < that.ur.y && ur.y > that.ll.y) {
                    if (ll.x > that.ll.x) { lowerLeft.x = ll.x; }
                    else { lowerLeft.x = that.ll.x; }

                    if (ll.y > that.ll.y) { lowerLeft.y = ll.y; }
                    else { lowerLeft.y = that.ll.y; }

                    if (ur.x < that.ur.x) { upperRight.x = ur.x; }
                    else { upperRight.x = that.ur.x; }

                    if (ur.y < that.ur.y) { upperRight.y = ur.y; }
                    else { upperRight.y = that.ur.y; }
                }
            }
        }

        return {lowerLeft.x, lowerLeft.y, upperRight.x, upperRight.y};
    }

    bool hasOverlap(const Rectangle& that, bool lineOverlap = false) const {
        return !overlap(that, lineOverlap).isZero();
    }

    bool isCoveredBy(const Rectangle& that) const {
        return that.ll.x <= ll.x &&
               that.ll.y <= ll.y &&
               that.ur.x >= ur.x &&
               that.ur.y >= ur.y;
    }

    bool isZero() const {
        double dx = ur.x - ll.x;
        double dy = ur.y - ll.y;
        double area = dx * dy;
        return fabs(area) < 1e-6;
    }

    std::string toString() const {
        std::string s;
        s = ll.toString() + " " + ur.toString();
        return s;
    }

    bool operator==(const Rectangle& that) const {
        return ll == that.ll && ur == that.ur;
    }

    bool operator!=(const Rectangle& that) const {
        return !(*this == that);
    }

public:
    Point ll;
    Point ur;
    double width;
    double height;
};

#endif // VLSI_FINAL_PROJECT_RECTANGLE_HPP_
