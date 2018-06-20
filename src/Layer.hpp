#ifndef VLSI_FINAL_PROJECT_LAYER_HPP_
#define VLSI_FINAL_PROJECT_LAYER_HPP_

#include <string>
#include <utility>

class Layer {
public:
    Layer() = default;

    Layer(std::string _name, int _spacing, std::string _direction) {
        name = std::move(_name);
        spacing = _spacing;
        direction = dir_str2char(_direction);
    }

    char dir_str2char(const std::string& s) {
        if (s == "horizontal") return 'H';

        if (s == "vertical") return 'V';
        return ' ';
    }

    bool isHorizontal() const {
        return direction == 'H';
    }

    bool isVertical() const {
        return direction == 'V';
    }

    std::string toString()
    {
        std::string s;
        s = name + " , " + std::to_string(spacing) + " , " + direction;
        return s;
    }

public:
    std::string name;
    int spacing;
    char direction;
};
#endif
