#ifndef VLSI_FIANL_PROJECT_BIT_HPP_
#define VLSI_FIANL_PROJECT_BIT_HPP_
#include <vector>
class Bit
{
public:
    Bit(){
        pins.clear();
    }
    vector<Pin> pins;
    void addPin(Pin p){
        pins.push_back(p);
    }

};

#endif