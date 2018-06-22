#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

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
        doRouting();
    }

private:
    void initialize() {

    }

    void doRouting() {
        for (const Net& net : nets) {
            routeSingleNet(net);
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
            Layer layer = vertices[i].track.layer;
            Point from = vertices[i].track.rect.ll;
            Point from = vertices[i].track.rect.ur;
        }
    }

    bool routeSingleNet(const Net& net) {
        std::vector<int> srcs;
        srcs.emplace_back(net.net[0]);
    }

    bool routeSinglePath(const std::vector<int>& src, int target) {

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
    Logger& logger;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
