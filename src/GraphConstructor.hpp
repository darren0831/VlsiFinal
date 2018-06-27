#ifndef VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

#include <algorithm>
#include <limits>
#include <thread>
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
#include "Net.hpp"
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
        initializeNets();
        writeLog();
        printRoutingGraph();
    }

    void preCalculate() {

        for (int i = 0; i < (int) layers.size(); i++) {
            int width = std::numeric_limits<int>::max();
            for (int j = 0; j < (int) buses.size(); j++) {
                width = std::min(buses[j].widths[i], width);
            }
            min_bus_width.emplace_back(width);
        }
    }

    void initialize() {
        logger.info("Graph constructor initialize\n");
        logger.info("Split track\n");
        splitTracks();
        logger.info("Generate vertices and edges\n");
        logger.info("Generate vertices based on tracks split\n");
        generateVertices();
        logger.info("Generate edges based on routing region\n");
        generateEdges();
    }

    void initializeNets() {
        logger.info("Initialize nets\n");
        int multiOverlapCount = 0;
        int totalTerminals = 0;
        int vertexId = (int) vertices.size();
        for (const auto& bus : buses) {
            Net net(bus.numBits);
            totalTerminals += bus.numBits * bus.numPins;
            for (int i = 0; i < bus.numBits; ++i) {
                for (const Pin& pin : bus.bits[i].pins) {
                    int layer = pin.layer;
                    const Rectangle& location = pin.rect;
                    std::vector<int> touchedVertices;
                    for (const Vertex& v : layerVertices[layer]) {
                        if (v.track.rect.hasOverlap(location)) {
                            touchedVertices.emplace_back(v.id);
                        }
                    }
                    if (touchedVertices.empty()) {
                        logger.error("A pin of bus %s doesn't touch any vertices\n", bus.name.c_str());
                    } else if (touchedVertices.size() == 1lu) {
                        net.addTerminal(i, touchedVertices[0]);
                    } else {
                        ++multiOverlapCount;
                        vertexMap[vertexId] = Vertex(Track(), vertexId);
                        vertices.emplace_back(Track(), vertexId);
                        std::vector<Edge> newEdges;
                        for (const int touched : touchedVertices) {
                            newEdges.emplace_back(vertexId, touched, ' ', 'S');
                            routingGraph[touched].emplace_back(touched, vertexId, ' ', 'S');
                        }
                        routingGraph.emplace_back(newEdges);
                        if (routingGraph[vertexId].size() != newEdges.size()) {
                            fprintf(stderr, "FUCK!\n");
                        }
                        net.addTerminal(i, vertexId);
                        ++vertexId;
                        if ((int) vertices.size() != vertexId) {
                            fprintf(stderr, "FUCK\n");
                        }
                    }
                }
            }
            net.widths = bus.widths;
            nets.emplace_back(net);
        }
        logger.warning("%d / %d pins of buses overlap with more than one vertex\n", multiOverlapCount, totalTerminals);
        logger.info("%d nets generated\n", (int) nets.size());
    }

    void writeLog() {
        int totalEdges = 0;
        for (const auto& edges : routingGraph) {
            totalEdges += (int) edges.size();
        }
        logger.info("After graph construction,\n");
        logger.info("  %lu vertices generated\n", vertices.size());
        logger.info("  %d edges generated\n", totalEdges);
    }

    void splitTracks() {
        layerObstacles = std::vector<std::vector<Obstacle>>(layers.size());
        for (const auto& o : obstacles) {
            layerObstacles[o.layer].emplace_back(o);
        }
        for (int i = 0; i < (int) layers.size(); ++i) {
            std::sort(layerObstacles[i].begin(), layerObstacles[i].end(), [](Obstacle a, Obstacle b) {
                const Rectangle& ra = a.rect;
                const Rectangle& rb = b.rect;
                return ra.ll.x < rb.ll.x;
            });
        }
        std::vector<Track> all_tracks;
        for (const auto& t : tracks) {
            // all_tracks.emplace_back(t);
            std::stack<Track> stack;
            stack.push(t);
            while (!stack.empty()) {
                Track topTrack = stack.top();
                stack.pop();
                bool hasOverlap = false;
                for (const auto& o : layerObstacles[topTrack.layer]) {
                    if (o.rect.ll.x > topTrack.rect.ur.x) {
                        break;
                    }
                    Rectangle overlap = topTrack.rect.overlap(o.rect, false);
                    if (!overlap.isZero()) {
                        hasOverlap = true;
                        splitTrack(topTrack, overlap, stack);
                        break;
                    }
                }
                if (!hasOverlap) {
                    all_tracks.emplace_back(topTrack);
                }
            }
        }
        logger.info("Track count(after split): %d\n", all_tracks.size());
        layerTracks = std::vector<std::vector<Track>>(layers.size());
        for (const auto& t : all_tracks) {
            layerTracks[t.layer].emplace_back(t);
        }
        logger.info("Track info\n");
        for (int i = 0; i < (int) layers.size(); ++i) {
            logger.info("Layer %d: %d tracks\n", i, layerTracks[i].size());
        }
    }

    void generateVertices() {
        layerVertices = std::vector<std::vector<Vertex>>(layers.size());
        int vertexId = 0;
        for (int i = 0; i < (int) layerTracks.size(); ++i) {
            if (layers[i].isHorizontal()) {
                std::sort(layerTracks[i].begin(), layerTracks[i].end(), [](Track a, Track b) {
                    return a.rect.ll.x < b.rect.ll.x;
                });
            } else {
                std::sort(layerTracks[i].begin(), layerTracks[i].end(), [](Track a, Track b) {
                    return a.rect.ll.y < b.rect.ll.y;
                });
            }
            for (const Track& t : layerTracks[i]) {
                vertexMap[vertexId] = Vertex(t, vertexId);
                vertices.emplace_back(t, vertexId);
                layerVertices[t.layer].emplace_back(t, vertexId);
                ++vertexId;
            }
        }
        logger.info("%d vertices generated\n", vertexId);
    }

    void generateEdges() {
        routingGraph = std::vector<std::vector<Edge>>(vertices.size());
        std::vector<std::thread> threads;
        int part = (int) vertices.size() / 3;
        threads.emplace_back(std::thread([=] {
            generateEdges(0, part);
        }));
        threads.emplace_back(std::thread([=] {
            generateEdges(part, part * 2);
        }));
        threads.emplace_back(std::thread([=] {
            generateEdges(part * 2, (int) vertices.size());
        }));
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
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

    void generateEdges(int start, int end) {
        for (int i = start; i < end; ++i) {
            const Vertex& v = vertices[i];
            int layer = v.track.layer;
            std::vector<Edge>& outVertices = routingGraph[v.id];
            SegmentMap map[2];
            map[0] = SegmentMap(v.track.rect);
            map[1] = SegmentMap(v.track.rect);
            scanOutVertices(v, layer, outVertices);
            for (int l = layer - 1; l >= 0; --l) {
                scanOutVertices(v, l, map[0], outVertices);
            }
            for (int l = layer + 1; l < (int) layers.size(); ++l) {
                scanOutVertices(v, l, map[1], outVertices);
            }
        }
    }

    void splitTrack(const Track& t, const Rectangle& overlap, std::stack<Track>& stack) {
        const Layer& layer = layers[t.layer];
        double a, b, c, d, e, f, g, h;
        a = t.rect.ll.x;
        b = t.rect.ll.y;
        c = t.rect.ur.x;
        d = t.rect.ur.y;
        e = overlap.ll.x;
        f = overlap.ll.y;
        g = overlap.ur.x;
        h = overlap.ur.y;
        if (layer.isHorizontal()) {
            if (!doubleEqual(a, e)) {
                Track t1(a, (b + d) / 2, e, (b + d) / 2, t.width, t.layer); //(a,b,e,d)
                stack.push(t1);
            }
            if (!doubleEqual(c, g)) {
                Track t2(g, (b + d) / 2, c, (b + d) / 2, t.width, t.layer); //(g,b,c,d)
                stack.push(t2);
            }
            if (!doubleEqual(b, f)) {
                Track t3(e, (b + f) / 2, g, (b + f) / 2, (f - b), t.layer); //(e,b,g,f)
                if ((f - b) >= min_bus_width[t.layer] + (double) layer.spacing / 2) {
                    stack.push(t3);
                }
            }
            if (!doubleEqual(d, h)) {
                Track t4(e, (h + d) / 2, g, (h + d) / 2, (d - h), t.layer); //(e,h,g,d)
                if ((d - h) >= min_bus_width[t.layer] + (double) layer.spacing / 2) {
                    stack.push(t4);
                }
            }

        } else if (layer.isVertical()) {
            if (!doubleEqual(b, f)) {
                Track t1((a + c) / 2, b, (a + c) / 2, f, t.width, t.layer); //(a,b,c,f)
                stack.push(t1);
            }
            if (!doubleEqual(d, h)) {
                Track t2((a + c) / 2, h, (a + c) / 2, d, t.width, t.layer); //(a,h,c,d)
                stack.push(t2);
            }
            if (!doubleEqual(a, e)) {
                Track t3((a + e) / 2, f, (a + e) / 2, h, (e - a), t.layer); //(a,f,e,h)
                if ((e - a) >= min_bus_width[t.layer] + (double) layer.spacing / 2) {
                    stack.push(t3);
                }
            }
            if (!doubleEqual(c, g)) {
                Track t4((g + c) / 2, f, (g + c) / 2, h, (c - g), t.layer); //(g,f,c,h)
                if ((c - g) >= min_bus_width[t.layer] + (double) layer.spacing / 2) {
                    stack.push(t4);
                }
            }
        }
    }

    void scanOutVertices(const Vertex& v, int targetLayer, std::vector<Edge>& out) {
        const Layer& layer = layers[targetLayer];
        const double threshold = min_bus_width[targetLayer];
        const Point src = v.track.rect.midPoint();
        for (const Vertex& u : layerVertices[targetLayer]) {
            if (v != u && v.hasOverlap(u, true)) {
                Rectangle r = v.overlap(u, true);
                if (layer.isHorizontal() && r.height > threshold) {
                    const Point tgt = u.track.rect.midPoint();
                    char edgeDirection = getEdgeDirection(src, tgt, v.track.layer, u.track.layer);
                    Edge edge(v.id, u.id, edgeDirection, layer.direction);
                    out.emplace_back(std::move(edge));
                }
                if (layer.isVertical() && r.width > threshold) {
                    const Point tgt = u.track.rect.midPoint();
                    char edgeDirection = getEdgeDirection(src, tgt, v.track.layer, u.track.layer);
                    Edge edge(v.id, u.id, edgeDirection, layer.direction);
                    out.emplace_back(std::move(edge));
                }
            }
        }
    }

    void scanOutVertices(const Vertex& v, int targetLayer, SegmentMap& map, std::vector<Edge>& out) {
        int beginLayer = std::min(v.track.layer, targetLayer);
        int endLayer = std::max(v.track.layer, targetLayer);
        const Point src = v.track.rect.midPoint();
        for (const Vertex& u : layerVertices[targetLayer]) {
            if (v != u && v.hasOverlap(u, true)) {
                Rectangle r = v.overlap(u, true);
                if (map.insert(r) && isValidEdge(r, beginLayer, endLayer)) {
                    const Point tgt = u.track.rect.midPoint();
                    char edgeDirection = getEdgeDirection(src, tgt, v.track.layer, u.track.layer);
                    Edge edge(v.id, u.id, edgeDirection, 'C');
                    out.emplace_back(std::move(edge));
                }
            }
        }
    }

    char getEdgeDirection(const Point& src, const Point& tgt, int srcLayer, int tgtLayer) const {
        if (srcLayer == tgtLayer) {
            char layerDirection = layers[srcLayer].direction;
            if (layerDirection == 'H') {
                if (src.x < tgt.x) {
                    return 'L';
                } else {
                    return 'R';
                }
            } else {
                if (src.y < tgt.y) {
                    return 'B';
                } else {
                    return 'F';
                }
            }
        } else {
            if (srcLayer < tgtLayer) {
                return 'U';
            } else {
                return 'D';
            }
        }
    }

    bool isValidEdge(const Rectangle& r, int src, int tgt) {
        for (int layer = src + 1; layer < tgt; ++layer) {
            for (const auto& o : layerObstacles[layer]) {
                if (o.rect.hasOverlap(r)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool doubleEqual(double a, double b) {
        return fabs(a - b) < 1e-6;
    }

    void printRoutingGraph(){
#ifdef VLSI_FINAL_PROJECT_DEBUG_FLAG
        Logger logger("Log/RoutingGraph.log");
        for(int i=0;i<(int)routingGraph.size();i++){
            logger.show("Vertex ID %d: <",i);
            for(int j=0;j<(int)routingGraph[i].size();j++){
                logger.show("%d ",routingGraph[i][j].getTarget());
            }
            logger.show(">\n");
        }
#endif
    }

public:
    std::vector<Layer>& layers;
    std::vector<Track>& tracks;
    std::vector<Bus>& buses;
    std::vector<Obstacle>& obstacles;

public:
    std::vector<std::vector<Obstacle>> layerObstacles;
    std::vector<std::vector<Track>> layerTracks;
    std::vector<std::vector<Vertex>> layerVertices;
    std::vector<Vertex> vertices;
    std::vector<Net> nets;

public:
    std::unordered_map<int, Vertex> vertexMap;
    std::vector<std::vector<Edge>> routingGraph;

public:
    std::vector<int> min_bus_width;

private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
