#ifndef VLSI_FIANL_PROJECT_LAYER_HPP_
#define VLSI_FIANL_PROJECT_LAYER_HPP_
#include <string>

class Layer {
public:
    Layer() {}
    Layer(std::string _name, int _spacing, std::string _direction) {
        name = _name;
        spacing = _spacing;
        direction = dir_str2char(_direction);
    }
    std::string name;
    int spacing;
    char direction;
    char dir_str2char(std::string s) {
        if (s == "horizontal") return 'H';

        if (s == "vertical") return 'V';
    }

};
#endif