#ifndef VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

#include <algorithm>
#include <unordered_map>
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
#include "SegmentMap.hpp"
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
        preCalculate();
        initialize();
    }
    void preCalculate() {

        for(int i=0;i<(int)layers.size();i++)
        {
            int width=0x7FFFFFFF;
            for(int j=0;j<(int)buses.size();j++)
            {
                width = std::min(buses[j].bus_width[i],width);
            }
            min_bus_width.emplace_back(width);
        }
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
                const Rectangle& ra = a.rect;
                const Rectangle& rb = b.rect;
                return ra.lower_left.x < rb.lower_left.x;
            });
        }
        std::vector<Track> all_tracks;
        for (const auto& t : tracks) {
            std::stack<Track> stack;
            stack.push(t);
            while (!stack.empty()) {
                Track t = stack.top();
                stack.pop();
                bool hasOverlap = false;
                for (const auto& o : layerObstacles[t.layer]) {
                    if (o.rect.lower_left.x > t.rect.upper_right.x) {
                        break;
                    }
                    Rectangle overlap = t.rect.overlap(o.rect,false);
                    if (!overlap.isZero()) {
                        hasOverlap = true;
                        splitTrack(t, overlap, stack);
                        break;
                    }
                }
                if (!hasOverlap) {
                    all_tracks.push_back(t);
                }
            }
        }
        logger.info("Track count(after split): %d\n",all_tracks.size());
        layerTracks = std::vector<std::vector<Track>>(layers.size());
        layerVertices = std::vector<std::vector<Vertex>>(layers.size());
        for (const auto& t : all_tracks) {
            layerTracks[t.layer].push_back(t);
        }
        logger.info("Track info\n");
        for (int i = 0; i < (int) layers.size(); ++i) {
            logger.info("Layer %d: %d tracks\n", i, layerTracks[i].size());
        }
        logger.info("Generate vertices and edges\n");
        logger.info("Generate vertices based on tracks split\n");
        int vertexId = 0;
        for (int i = 0; i < (int) layerTracks.size(); ++i) {
            if (layers[i].isHorizontal()) {
                std::sort(layerTracks[i].begin(), layerTracks[i].end(), [](Track a, Track b) {
                    return a.rect.lower_left.x < b.rect.lower_left.x;
                });
            } else {
                std::sort(layerTracks[i].begin(), layerTracks[i].end(), [](Track a, Track b) {
                    return a.rect.lower_left.y < b.rect.lower_left.y;
                });
            }
            for (const Track& t : layerTracks[i]) {
                vertexMap[vertexId] = Vertex(t, vertexId);
                vertices.push_back(Vertex(t, vertexId));
                layerVertices[t.layer].push_back(Vertex(t, vertexId));
                ++vertexId;
            }
        }
        logger.info("%d vertices generated\n", vertexId);
        logger.info("Generate edges based on routing region\n");
        routingGraph = std::vector<std::vector<Vertex>>(vertices.size());
        for (const Vertex& v : vertices) {
            int layer = v.track.layer;
            std::vector<Vertex>& outVertices = routingGraph[v.id];
            SegmentMap map[3];
            map[0] = SegmentMap(v.track.rect);
            map[1] = SegmentMap(v.track.rect);
            map[2] = SegmentMap(v.track.rect);
            scanOutVertices(v, layer, map[0], outVertices);
            for (int l = layer - 1; l >= 0; --l) {
                scanOutVertices(v, l, map[1], outVertices);
            }
            for (int l = layer + 1; l < (int) layers.size(); ++l) {
                scanOutVertices(v, l, map[2], outVertices);
            }
        }
        int edgeCount = 0;
        double avgEdgeCount = 0;
        for (const auto& v : routingGraph) {
            edgeCount += (int) v.size();
        }
        avgEdgeCount = ((double) edgeCount) / routingGraph.size();
        logger.info("%d edges generated, average %f edge for each vertex\n", edgeCount, avgEdgeCount);
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
        if(layer.isHorizontal()){
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
                Track t3(e,(b+f)/2,g,(b+f)/2,(f-b),t.layer); //(e,b,g,f)
                if((f-b)>=min_bus_width[t.layer]+layer.spacing/2)
                    stack.push(t3);
            }
            if (!doubleEqual(d, h))
            {
                Track t4(e,(h+d)/2,g,(h+d)/2,(d-h),t.layer); //(e,h,g,d)
                if((d-h)>=min_bus_width[t.layer]+layer.spacing/2)
                    stack.push(t4);
            }

        }else if(layer.isVertical())
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
                Track t3((a+e)/2,f,(a+e)/2,h,(e-a),t.layer); //(a,f,e,h)
                if((e-a)>=min_bus_width[t.layer]+layer.spacing/2)
                    stack.push(t3);
            }
            if (!doubleEqual(c, g))
            {
                Track t4((g+c)/2,f,(g+c)/2,h,(c-g),t.layer); //(g,f,c,h)
                if((c-g)>=min_bus_width[t.layer]+layer.spacing/2)
                    stack.push(t4);
            }
        }
    }

    void scanOutVertices(const Vertex& v, int targetLayer, SegmentMap& map, std::vector<Vertex>& out) {
        for (const Vertex& u : layerVertices[targetLayer]) {
            if (v != u && v.hasOverlap(u,true)) {
                Rectangle r = v.overlap(u,true);
                if (map.insert(r)) {
                    out.push_back(v);
                }
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
    std::vector<std::vector<Track>> layerTracks;
    std::vector<std::vector<Vertex>> layerVertices;
    std::vector<Vertex> vertices;

public:
    std::unordered_map<int, Vertex> vertexMap;
    std::vector<std::vector<Vertex>> routingGraph;
public:
    std::vector<int> min_bus_width;
private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
