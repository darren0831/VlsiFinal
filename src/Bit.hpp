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

    Bit(const Bit& that) {
        name = that.name;
        pins = that.pins;
    }

    Bit(Bit&& that) noexcept {
        name = std::move(that.name);
        pins = std::move(that.pins);
    }

    Bit& operator=(const Bit& that) {
        if (this != &that) {
            name = that.name;
            pins = that.pins;
        }
        return *this;
    }

    Bit& operator=(Bit&& that) noexcept {
        if (this != &that) {
            name = std::move(that.name);
            pins = std::move(that.pins);
        }
        return *this;
    }

    void addPin(const Pin& p) {
        pins.push_back(p);
    }

    std::string toString() const {
        std::string s;
        s += name + "\n";
        for (const auto& pin : pins) {
            s += std::string("\t") + pin.toString() + std::string("\n");
        }
        return s;
    }

public:
    std::string name;
    std::vector<Pin> pins;
};

#endif // VLSI_FINAL_PROJECT_BIT_HPP_
