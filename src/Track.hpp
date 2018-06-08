#ifndef VLSI_FIANL_PROJECT_TRACK_HPP_
#define VLSI_FIANL_PROJECT_TRACK_HPP_
#include <string>
#include "Rectangle.hpp"

class Track
{
public:
	Track(double _lx,double _ly,double _rx,double _ry,double _w, int _layer){
		rect = Rectangle(_lx,_ly-_w/2,_rx,_ry+_w/2);
		width = _w;
		layer= _layer;
	}
	Rectangle rect;
	double width;
	int layer;
};

#endif
