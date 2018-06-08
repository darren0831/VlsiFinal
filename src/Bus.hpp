#ifndef VLSI_FIANL_PROJECT_BUS_HPP_
#define VLSI_FIANL_PROJECT_BUS_HPP_
#include <map>
#include <vector>
#include "Bit.hpp"
class Bus{
public:
    Bus(){
        bus_width.clear();
        bits.clear();
    }
    vector<Bit> bits;
    std::string name;
    int bit_counts;
    int pin_counts;
    map<int,int> bus_width;
    void addBit(Bit b){
        bits.push_back(b);
    }

};
#endif