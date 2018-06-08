#ifndef VLSI_FIANL_PROJECT_BUS_HPP_
#define VLSI_FIANL_PROJECT_BUS_HPP_
#include <map>
#include <vector>
#include <string>
#include "Bit.hpp"
class Bus{
public:
    Bus(std::string _name, const std::vector<int>& _bus_width, int numBits, int numPins){
        name = _name;
        bus_width = _bus_width;
        bit_counts = numBits;
        pin_counts = numPins;
    }
    std::vector<Bit> bits;
    std::string name;
    int bit_counts;
    int pin_counts;
    std::vector<int> bus_width;
    void addBit(Bit b){
        bits.push_back(b);
    }

};
#endif