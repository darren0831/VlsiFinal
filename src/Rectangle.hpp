#ifndef VLSI_FINAL_PROJECT_RECTANGLE_HPP_
#define VLSI_FINAL_PROJECT_RECTANGLE_HPP_

#include <cmath>
#include <string>
#include "Point.hpp"

class Rectangle {
public:
    Rectangle() {}
    Rectangle(double lf_x, double lf_y, double ur_x, double ur_y) {
        lower_left = Point(lf_x, lf_y);
        upper_right = Point(ur_x, ur_y);
    }

    Point lower_left, upper_right;

    Rectangle overlapWith(const Rectangle& that) const {
        Point ll = Point(0, 0), ur = Point(0, 0);

        if(lower_left.x<that.upper_right.x&&upper_right.x>that.lower_left.x){
            if(lower_left.y<that.upper_right.y&&upper_right.y>that.lower_left.y){
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
        return Rectangle(ll.x, ll.y, ur.x, ur.y);
    }

    bool hasOverlapWith(const Rectangle& that) const {
        return !overlapWith(that).isZero();
    }

    bool isZero() const {
        return std::fpclassify(lower_left.x) == FP_ZERO &&
            std::fpclassify(lower_left.y) == FP_ZERO &&
            std::fpclassify(upper_right.x) == FP_ZERO &&
            std::fpclassify(upper_right.y) == FP_ZERO;
    }

    std::string print()
    {
    	std::string s;
    	s = lower_left.print()+" "+upper_right.print();
    	return s;
    }
};

#endif
