#ifndef VLSI_FINAL_PROJECT_LAYER_HPP_
#define VLSI_FINAL_PROJECT_LAYER_HPP_

#include <string>
#include <utility>

class Layer {
public:
    Layer() = default;

    Layer(std::string name, int spacing, const std::string& direction) :
        name(std::move(name)), spacing(spacing), direction(directionToChar(direction)) {}

    char directionToChar(const std::string& s) const {
        if (s == "horizontal") { return 'H'; }

        if (s == "vertical") { return 'V'; }
        return ' ';
    }

    bool isHorizontal() const {
        return direction == 'H';
    }

    bool isVertical() const {
        return direction == 'V';
    }

    std::string toString() const {
        std::string s;
        s = name + " , " + std::to_string(spacing) + " , " + direction;
        return s;
    }

public:
    std::string name;
    int spacing;
    char direction;
};

#endif // VLSI_FINAL_PROJECT_LAYER_HPP_
