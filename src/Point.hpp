#ifndef VLSI_FINAL_PROJECT_POINT_HPP_
#define VLSI_FINAL_PROJECT_POINT_HPP_
#include <string>
class Point {
public:
    Point(): x(0), y(0) {}

    Point (double _x, double _y): x(_x), y(_y) {}

    std::string print()
    {
    	std::string s;
    	s = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
    	return s;
    }

public:
    double x;
    double y;
};

#endif
