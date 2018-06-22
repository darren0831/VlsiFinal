#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <iterator>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Vertex.hpp"

class GlobalRouter {
private:
    class GlobalEdge {
    public:
        GlobalEdge(int id,int src,int tgt):id(id),src(src),tgt(tgt) {}
        double getCost() {return cost;}
        void setCost(double cost) {this->cost = cost;}
        double getHistoricalCost() {return historical_cost;}
        void setHistoricalCost(double historical_cost) {this->historical_cost = historical_cost;}
        int src;
        int tgt;
        int id;
        int capacity;
        int edgeRequest(int count, double width) {

        }
        void edgeRecover(int operId) {

        }
        int edgeCount(double querywidth) {

        }
        void addtracks(Vertex v) {
            vertices[v.id] = v.track.width;
        }

    private:
        double cost;
        double historical_cost;
        std::unordered_map<int,double> vertices;


    };
public:
    GlobalRouter(std::vector<Vertex>& vertices,
                 std::unordered_map<int, Vertex>& vertexMap,
                 std::vector<std::vector<Vertex>>& routingGraph,
                 std::vector<Net>& nets,
                 std::vector<Bus>& buses,
                 Logger& logger) :
        vertices(vertices), vertexMap(vertexMap), routingGraph(routingGraph), nets(nets), buses(buses), logger(logger) {}

    void globalRoute() {
        initialize();
        prepareNets();
        doRouting();
    }

private:
    void initialize() {

    }

    void prepareNets() {
        globalNets = std::vector<std::vector<int>>();
        globalNetWidths = std::vector<std::vector<int>>();
        for (const Bus& bus : buses) {
            std::vector<int> net;
            std::vector<int> width;
            for (int i = 0; i < (int) bus.widths.size(); ++i) {
                width.emplace_back(bus.widths[i] * bus.numBits);
            }
            std::vector<int> grids;
            for (int i = 0; i < bus.numPins; ++i) {
                std::vector<int> pinGrids;
                for (int j = 0; j < bus.numBits; ++j) {
                    const Pin& pin = bus.bits[j].pins[i];
                    Point midPoint = pin.rect.midPoint();
                    int gridId = coordToGridId(midPoint, pin.layer);
                    pinGrids.emplace_back(gridId);
                }
                std::unordered_map<int, int> countMap;
                for (int v : pinGrids) {
                    countMap[v]++;
                }
                int maxid = -1;
                int maxv = 0;
                for (const auto& vp : countMap) {
                    if (vp.second > maxv) {
                        maxv = vp.second;
                        maxid = vp.first;
                    }
                }
                grids.emplace_back(maxid);
            }
            globalNets.emplace_back(std::move(net));
            globalNetWidths.emplace_back(std::move(width));
        }
    }

    void doRouting() {
        for (int i = 0; i < (int) globalNets.size(); ++i) {
            routeSingleNet(globalNets[i], globalNetWidths[i]);
        }
    }

    int calGridWidth() {
        std::vector<int> medians;
        for(int i=0;i<(int)buses.size();i++){
            std::vector<int> s = buses[i].widths;
            std::sort(s.begin(),s.end());
            medians.emplace_back(buses[i].widths[buses[i].widths.size()/2]);
        }
        std::sort(medians.begin(),medians.end());
        return medians[medians.size()/2];
    }

    void implementGlobalGrid() {
        int gridWidth = calGridWidth();
        for(int i=0;i<(int)vertices.size();i++){
        }
    }

    bool routeSingleNet(const std::vector<int>& net, const std::vector<int>& widths) {
        std::vector<int> tgt;
        tgt.emplace_back(net.at(0));
        for (int i = 1; i < (int) net.size(); ++i) {
            int src = net[i];
            auto result = routeSinglePath(src, tgt, widths);
            if (!result.empty()) {
                for (int v : result) {
                    tgt.emplace_back(v);
                }
            } else {
                logger.error("Fuck! Do rip-up reroute\n");
                return false;
            }
        }
        return true;
    }

    std::vector<int> routeSinglePath(const int src, const std::vector<int> tgt, const std::vector<int>& width) {

    }

private:
    std::vector<Vertex>& vertices;
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;
    std::vector<Net>& nets;
    std::vector<Bus>& buses;

private:
    std::vector<std::vector<int>> globalGraph;
    std::vector<GlobalEdge> globalEdges;

private:
    std::vector<std::vector<int>> globalNets;
    std::vector<std::vector<int>> globalNetWidths;

private:
    Logger& logger;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
