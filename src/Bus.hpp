#ifndef VLSI_FINAL_PROJECT_BUS_HPP_
#define VLSI_FINAL_PROJECT_BUS_HPP_

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include "Bit.hpp"

class Bus {
public:
    Bus(std::string name, std::vector<int> widths, int numBits, int numPins) :
        name(std::move(name)), widths(std::move(widths)), numBits(numBits), numPins(numPins) {}

    void addBit(const Bit& b) {
        bits.push_back(b);
    }

    std::string toString() const {
        std::string s;
        s += name + std::string("\n");
        s += std::string("WIDTH: \n");
        for (unsigned i = 0; i < widths.size(); i++) {
            s += std::string("\t") + std::string("layer: ") + std::to_string(i) + " = " + std::to_string(widths[i]) +
                 "\n";
        }
        s += std::string("BITS: \n");
        for (unsigned i = 0; i < bits.size(); i++) {
            s += std::string("\t") + bits[i].toString() + std::string("\n");
        }
        return s;
    }

public:
    std::string name;
    std::vector<Bit> bits;
    std::vector<int> widths;
    int numBits;
    int numPins;
};

#endif // VLSI_FINAL_PROJECT_BUS_HPP_
