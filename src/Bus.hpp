#ifndef VLSI_FINAL_PROJECT_BUS_HPP_
#define VLSI_FINAL_PROJECT_BUS_HPP_
#include <map>
#include <vector>
#include <string>
#include "Bit.hpp"
class Bus {
public:
    Bus(std::string _name, const std::vector<int>& _bus_width, int numBits, int numPins) {
        name = _name;
        bus_width = _bus_width;
        bit_counts = numBits;
        pin_counts = numPins;
    }

    void addBit(Bit b) {
        bits.push_back(b);
    }

    std::string print()
    {
        std::string s="";
        s+=name+std::string("\n");
        s+=std::string("WIDTH: \n");
        for(unsigned i=0;i<bus_width.size();i++)
        {
            s+=std::string("\t") + std::string("layer: ") + std::to_string(i) + " = " + std::to_string(bus_width[i]) + "\n";
        }
        s+=std::string("BITS: \n");
        for(unsigned i=0;i<bits.size();i++)
        {
            s+=std::string("\t")+bits[i].print()+std::string("\n");
        }
        return s;
    }

public:
    std::vector<Bit> bits;
    std::vector<int> bus_width;
    std::string name;
    int bit_counts;
    int pin_counts;
};
#endif
