#ifndef VLSI_FINAL_PROJECT_RECTANGLE_HPP_
#define VLSI_FINAL_PROJECT_RECTANGLE_HPP_

#include <cmath>
#include <initializer_list>
#include <string>
#include "Point.hpp"

class Rectangle {
public:
    Rectangle() = default;

    Rectangle(double lf_x, double lf_y, double ur_x, double ur_y) {
        lower_left = Point(lf_x, lf_y);
        upper_right = Point(ur_x, ur_y);
    }

    Rectangle overlap(const Rectangle& that) const {
        Point ll = Point(0, 0), ur = Point(0, 0);

        if(lower_left.x<=that.upper_right.x&&upper_right.x>=that.lower_left.x){
            if(lower_left.y<=that.upper_right.y&&upper_right.y>=that.lower_left.y){
                if(lower_left.x>that.lower_left.x) ll.x = lower_left.x;
                else ll.x = that.lower_left.x;
                if(lower_left.y>that.lower_left.y) ll.y = lower_left.y;
                else ll.y = that.lower_left.y;
                if(upper_right.x<that.upper_right.x) ur.x = upper_right.x;
                else ur.x = that.upper_right.x;
                if(upper_right.y<that.upper_right.y) ur.y = upper_right.y;
                else ur.y = that.upper_right.y;
            }
        }
        return {ll.x, ll.y, ur.x, ur.y};
    }

    bool hasOverlap(const Rectangle& that) const {
        return !overlap(that).isZero();
    }

    bool isCoveredBy(const Rectangle& that) const {
        return that.lower_left.x <= lower_left.x &&
               that.lower_left.y <= lower_left.y &&
               that.upper_right.x >= upper_right.x &&
               that.upper_right.y >= upper_right.y;
    }

    bool isZero() const {
        return std::fpclassify(lower_left.x) == FP_ZERO &&
            std::fpclassify(lower_left.y) == FP_ZERO &&
            std::fpclassify(upper_right.x) == FP_ZERO &&
            std::fpclassify(upper_right.y) == FP_ZERO;
    }

    std::string toString()
    {
    	std::string s;
    	s = lower_left.toString()+" "+ upper_right.toString();
    	return s;
    }

    bool operator==(const Rectangle& that) const {
        return lower_left == that.lower_left && upper_right == that.upper_right;
    }

    bool operator!=(const Rectangle& that) const {
        return !(*this == that);
    }

public:
    Point lower_left;
    Point upper_right;
};

#endif
