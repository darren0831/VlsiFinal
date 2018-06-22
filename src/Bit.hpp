#ifndef VLSI_FINAL_PROJECT_BIT_HPP_
#define VLSI_FINAL_PROJECT_BIT_HPP_

#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include "Pin.hpp"

class Bit {
public:
    explicit Bit(std::string name) :
        name(std::move(name)), pins(std::vector<Pin>()) {}

    void addPin(Pin p) {
        pins.push_back(p);
    }

    std::string toString() const {
        std::string s;
        s += name + "\n";
        for (unsigned i = 0; i < pins.size(); i++) {
            s += std::string("\t") + pins[i].toString() + std::string("\n");
        }
        return s;
    }

public:
    std::string name;
    std::vector<Pin> pins;
};

#endif // VLSI_FINAL_PROJECT_BIT_HPP_
