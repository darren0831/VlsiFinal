#ifndef VLSI_FIANL_PROJECT_BIT_HPP_
#define VLSI_FIANL_PROJECT_BIT_HPP_
#include <vector>
#include <string>
#include "Pin.hpp"
class Bit
{
public:
    Bit(std::string _name){
    	name = _name;
    }
    std::vector<Pin> pins;
    std::string name; 
    void addPin(Pin p){
        pins.push_back(p);
    }

};

#endif