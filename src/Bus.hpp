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

    Bus(const Bus& that) {
        name = that.name;
        bits = that.bits;
        widths = that.widths;
        numBits = that.numBits;
        numPins = that.numPins;
    }

    Bus(Bus&& that) noexcept {
        name = std::move(that.name);
        bits = std::move(that.bits);
        widths = std::move(that.widths);
        numBits = that.numBits;
        numPins = that.numPins;
    }

    Bus& operator=(const Bus& that) {
        if (this != &that) {
            name = that.name;
            bits = that.bits;
            widths = that.widths;
            numBits = that.numBits;
            numPins = that.numPins;
        }
        return *this;
    }

    Bus& operator=(Bus&& that) noexcept {
        if (this != &that) {
            name = std::move(that.name);
            bits = std::move(that.bits);
            widths = std::move(that.widths);
            numBits = that.numBits;
            numPins = that.numPins;
        }
        return *this;
    }

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
        for (const auto& bit : bits) {
            s += std::string("\t") + bit.toString() + std::string("\n");
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
