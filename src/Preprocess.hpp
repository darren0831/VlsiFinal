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
    Preprocess(std::vector<Layer>& layers, std::vector<Obstacle>& obstacles, Rectangle& boundary) :
            layers(layers), obstacles(obstacles), boundary(boundary) {
        obstaclePreprocess();
    }

    void obstaclePreprocess() {
        for (auto& o : obstacles) {
            double spacing = layers[o.layer].spacing;
            Rectangle origin = o.rect;
            Rectangle cur;
            if(layers[o.layer].isHorizontal()){
                cur = Rectangle(origin.ll.x,
                                origin.ll.y - spacing / 2,
                                origin.ur.x,
                                origin.ur.y + spacing / 2);  
            }else{
                cur = Rectangle(origin.ll.x - spacing / 2,
                                origin.ll.y,
                                origin.ur.x + spacing / 2,
                                origin.ur.y);
            }
            
            o.rect = cur;
        }
        double bx = boundary.ur.x;
        double by = boundary.ur.y;
        for(int i=0;i<(int)layers.size();i++){
            double spacing = layers[i].spacing;
            obstacles.emplace_back(Obstacle(i,0,0,spacing/2,by));
            obstacles.emplace_back(Obstacle(i,0,by-spacing/2,bx,by));
            obstacles.emplace_back(Obstacle(i,0,0,bx,spacing/2));
            obstacles.emplace_back(Obstacle(i,bx-spacing/2,0,bx,by));
        }
        
    }

public:
    std::vector<Layer>& layers;
    std::vector<Obstacle>& obstacles;
    Rectangle& boundary;
};

#endif //VLSI_FINAL_PROJECT_INPUT_PREPROCESS_HPP_
