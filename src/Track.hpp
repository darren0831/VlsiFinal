#ifndef VLSI_FIANL_PROJECT_TRACK_HPP_
#define VLSI_FIANL_PROJECT_TRACK_HPP_
class Track
{
public:
    Track(){};
    void initial(double _lx,double _ly,double _rx,double _ry,double _w,std::string _name,int _layer){
        rect(_lx,_ly-_w/2,_rx,_ry+_w/2);
        name = _name;
        layer = _layer;
    }
	Rectangle rect;
	std::string name;
	int layer;
};

#endif
