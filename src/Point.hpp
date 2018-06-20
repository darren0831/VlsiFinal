#ifndef VLSI_FINAL_PROJECT_POINT_HPP_
#define VLSI_FINAL_PROJECT_POINT_HPP_

#include <string>

class Point {
public:
    Point(): x(0), y(0) {}

    Point (double _x, double _y): x(_x), y(_y) {}

    std::string toString()
    {
    	std::string s;
    	s = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
    	return s;
    }

    bool operator==(const Point& that) const {
        double dx = fabs(x - that.x);
        double dy = fabs(y - that.y);
        return dx < 1e-6 && dy < 1e-6;
    }

    bool operator!=(const Point& that) const {
        return !(*this == that);
    }

public:
    double x;
    double y;
};

#endif
