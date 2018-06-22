#ifndef VLSI_FINAL_PROJECT_INPUT_PREPROCESS_HPP_
#define VLSI_FINAL_PROJECT_INPUT_PREPROCESS_HPP_

#include <vector>
#include "Layer.hpp"
#include "Track.hpp"
#include "Obstacle.hpp"

/**
 * NOTE: This class is use for preprocess beofer construct graph
 * 		 1. make obstacle bigger to fit spacing problem
 */
class Preprocess {
public:
    Preprocess(std::vector<Layer>& layers, std::vector<Obstacle>& obstacles) :
            layers(layers), obstacles(obstacles) {
        obstaclePreprocess();
    }

    void obstaclePreprocess() {
        for (auto& o : obstacles) {
            double spacing = layers[o.layer].spacing;
            Rectangle origin = o.rect;
            Rectangle cur = Rectangle(origin.ll.x - spacing / 2,
                                      origin.ll.y - spacing / 2,
                                      origin.ur.x + spacing / 2,
                                      origin.ur.y + spacing / 2);
            o.rect = cur;
        }
    }

public:
    std::vector<Layer>& layers;
    std::vector<Obstacle>& obstacles;
};

#endif //VLSI_FINAL_PROJECT_INPUT_PREPROCESS_HPP_
