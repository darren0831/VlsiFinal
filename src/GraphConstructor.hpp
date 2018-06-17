#ifndef VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

#include <algorithm>
#include <vector>
#include <stack>
#include "Bit.hpp"
#include "Bus.hpp"
#include "Edge.hpp"
#include "GraphConstructor.hpp"
#include "InputReader.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Obstacle.hpp"
#include "Pin.hpp"
#include "Point.hpp"
#include "Rectangle.hpp"
#include "Track.hpp"
#include "Vertex.hpp"

class GraphConstructor {
public:
    GraphConstructor(std::vector<Layer>& layers,
        std::vector<Track>& tracks,
        std::vector<Bus>& buses,
        std::vector<Obstacle>& obstacles,
        Logger& logger) :
        layers(layers), tracks(tracks), buses(buses), obstacles(obstacles), logger(logger) {
        initialize();
    }

    void initialize() {
        logger.info("Graph constructor initialize\n");
        logger.info("Split track\n");
        std::vector<std::vector<Obstacle>> layerObstacles(layers.size());
        for (const auto& o : obstacles) {
            layerObstacles[o.layer].push_back(o);
        }
        for (int i = 0; i < (int) layers.size(); ++i) {
            std::sort(layerObstacles[i].begin(), layerObstacles[i].end(), [](Obstacle a, Obstacle b) {
                const Rectangle& ra = a.area;
                const Rectangle& rb = b.area;
                return ra.lower_left.x < rb.lower_left.x;
            });
        }
        std::vector<Track> all_tracks;
        int c = 0;
        for (const auto& t : tracks) {
            printf("\r %d / %lu", ++c, tracks.size());
            fflush(stdout);
            std::stack<Track> stack;
            stack.push(t);
            while (!stack.empty()) {
                Track t = stack.top();
                stack.pop();
                bool hasOverlap = false;
                for (const auto& o : layerObstacles[t.layer]) {
                    if (o.area.lower_left.x > t.rect.upper_right.x) {
                        break;
                    }
                    Rectangle overlap = t.rect.overlapWith(o.area);
                    if (!overlap.isZero()) {
                        hasOverlap = true;
                        splitTrack(t, overlap, stack);
                    }
                }
                if (!hasOverlap) {
                    all_tracks.push_back(t);
                }
            }
        }
        std::vector<std::vector<Track>> layerTracks(layers.size());
        for (const auto& t : all_tracks) {
            layerTracks[t.layer].push_back(t);
        }
        logger.info("Track info\n");
        for (int i = 0; i < (int) layers.size(); ++i) {
            logger.info("Layer %d: %d tracks\n", i, layerTracks[i].size());
        }
        logger.info("Generate vertices and edges\n");
        int vertexId = 0;
        for (unsigned i = 0; i < layers.size(); ++i) {
            for (unsigned j = i + 1; j < layers.size(); ++j) {
                for (const Track& ta : layerTracks[i]) {
                    for (const Track& tb : layerTracks[j]) {
                        if (ta.rect.hasOverlapWith(tb.rect)) {
                            ++vertexId;
                        }
                    }
                }
            }
        }
        // int vertextId=0;
        // for(unsigned i=0;i<all_tracks.size();i++)
        // {
        //     for(unsigned j=i+1;j<all_tracks.size();j++)
        //     {
        //         Rectangle overlap = all_tracks[i].rect.overlapWith(all_tracks[j].rect);
        //         if(!overlap.isZero())   //two track overlap
        //         {
        //             int t1_layer_id = all_tracks[i].layer;
        //             int t2_layer_id = all_tracks[j].layer;
        //             const Layer& t1_layer = layers[t1_layer_id];
        //             const Layer& t2_layer = layers[t2_layer_id];
        //             if(t1_layer.direction==t2_layer.direction)
        //             {
        //                 if(all_tracks[i].width>all_tracks[j].width)
        //                 {
        //                     Rectangle large_overlap = largeOverlap(overlap,all_tracks[i].width,t1_layer.direction);
        //                     ++vertextId;
        //                     if(t1_layer_id!=t2_layer_id)
        //                     {
        //                         ++vertextId;
        //                     }

        //                 }else if(all_tracks[i].width<all_tracks[j].width)
        //                 {
        //                     Rectangle large_overlap = largeOverlap(overlap,all_tracks[j].width,t1_layer.direction);
        //                     ++vertextId;
        //                     if(t1_layer_id!=t2_layer_id)
        //                     {
        //                         ++vertextId;
        //                     }
        //                 }else
        //                 {
        //                     if(t1_layer_id==t2_layer_id)
        //                     {
        //                         ++vertextId;
        //                     }else
        //                     {
        //                         vertextId += 2;
        //                     }
        //                 }
        //             }else
        //             {
        //                 vertextId += 2;
        //             }

        //         }
        //     }
        // }
        logger.info("%d vertices generated\n", vertexId);
    }

    Rectangle largeOverlap(Rectangle& overlap, double width, char direction)
    {
        Rectangle out;
        double a,b,c,d;
        a = overlap.lower_left.x;
        b = overlap.lower_left.y;
        c = overlap.upper_right.x;
        d = overlap.upper_right.y;
        if(direction=='H')
        {
            out = Rectangle(a,(b+d)/2-width/2,c,(b+d)/2+width/2);
        }else if(direction=='V')
        {
            out = Rectangle((a+c)/2-width/2,b,(a+c)/2-width/2,d);
        }
        return out;
    }

    void splitTrack(const Track& t, const Rectangle& overlap, std::stack<Track>& stack) {
        Layer layer = layers[t.layer];
        double a,b,c,d,e,f,g,h;
        a = t.rect.lower_left.x;
        b = t.rect.lower_left.y;
        c = t.rect.upper_right.x;
        d = t.rect.upper_right.y;
        e = overlap.lower_left.x;
        f = overlap.lower_left.y;
        g = overlap.upper_right.x;
        h = overlap.upper_right.y;
        if(layer.direction=='H'){
            if (!doubleEqual(a, e))
            {
                Track t1(a,(b+d)/2,e,(b+d)/2,t.width,t.layer); //(a,b,e,d)
                stack.push(t1);
            }
            if (!doubleEqual(c, g))
            {
                Track t2(g,(b+d)/2,c,(b+d)/2,t.width,t.layer); //(g,b,c,d)
                stack.push(t2);
            }
            if (!doubleEqual(b, f))
            {
                Track t3(e,(b+f)/2,g,(b+f)/2,(double)(f-b),t.layer); //(e,b,g,f)
                stack.push(t3);
            }
            if (!doubleEqual(d, h))
            {
                Track t4(e,(h+d)/2,g,(h+d)/2,(double)(d-h),t.layer); //(e,h,g,d)
                stack.push(t4);
            }

        }else if(layer.direction=='V')
        {
            if (!doubleEqual(b, f))
            {
                Track t1((a+c)/2,b,(a+c)/2,f,t.width,t.layer); //(a,b,c,f)
                stack.push(t1);
            }
            if (!doubleEqual(d, h))
            {
                Track t2((a+c)/2,h,(a+c)/2,d,t.width,t.layer); //(a,h,c,d)
                stack.push(t2);
            }
            if (!doubleEqual(a, e))
            {
                Track t3((a+e)/2,f,(a+e)/2,h,(double)(e-a),t.layer); //(a,f,e,h)
                stack.push(t3);
            }
            if (!doubleEqual(c, g))
            {
                Track t4((g+c)/2,f,(g+c)/2,h,(double)(c-g),t.layer); //(g,f,c,h)
                stack.push(t4);
            }
        }
    }

    bool doubleEqual(double a, double b) {
        return fabs(a - b) < 1e-6;
    }

public:
    std::vector<Layer>& layers;
    std::vector<Track>& tracks;
    std::vector<Bus>& buses;
    std::vector<Obstacle>& obstacles;

public:
    std::vector<Vertex> vertices;

private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
