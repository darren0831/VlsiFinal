#ifndef VLSI_FIANL_PROJECT_POINT_HPP_
#define VLSI_FIANL_PROJECT_POINT_HPP_
#include <string>
class Point {
public:
    Point(): x(0), y(0) {}
    Point (int _x, int _y): x(_x), y(_y) {}
    int x, y;
    std::string print()
    {
    	std::string s;
    	s = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
    	return s;
    }
};

#endif