#ifndef VLSI_FIANL_PROJECT_RECTANGLE_HPP_
#define VLSI_FIANL_PROJECT_RECTANGLE_HPP_
#include "Point.hpp"
#include <string>
class Rectangle {
public:
    Rectangle() {}
    Rectangle(int lf_x, int lf_y, int ur_x, int ur_y) {
        lower_left = Point(lf_x, lf_y);
        upper_right = Point(ur_x, ur_y);
    }

    Point lower_left, upper_right;

    Rectangle overlapWith(const Rectangle& that) {
        return Rectangle(0, 0, 0, 0);
    }

    std::string print()
    {
    	std::string s;
    	s = lower_left.print()+" "+upper_right.print();
    	return s;
    }
};

#endif
