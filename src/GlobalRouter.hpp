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

    std::vector<int> routeSinglePath(const int src, std::vector<int>& target,std::vector<int>& width,int bitCount) {
        std::vector<int> vertexCandidates;
        std::vector<bool> visited(globalGraph.size());
        std::vector<int> perdecessor(globalGraph.size());
        std::vector<int> gridCost(globalGraph.size());
        for(bool &i: visited) i = false;
        vertexCandidates.emplace_back(src);
        visited[src] = true;
        gridCost[src] = 0;
        perdecessor[src] = src;

        int nextId;
        double lowestCost = -1;
        bool foundTarget = false;
        while(!foundTarget){
            for(int &cur : vertexCandidates){
                for(int &next: globalGraph[cur]){
                    int nextVertex = globalEdges[next].tgt;
                    if(visited[nextVertex]==true) continue;
                    if(globalEdges[next].edgeCount(width[globalEdges[next].layer])<bitCount) continue;
                    if(gridCost[cur] + globalEdges[next].getCost() < lowestCost || lowestCost==-1){
                        nextId = nextVertex;
                        perdecessor[nextVertex] = cur;
                        lowestCost = gridCost[cur] + globalEdges[next].getCost();
                    }
                }
            }

            ///update grid cost and MST
            if(lowestCost != -1){
                vertexCandidates.emplace_back(nextId);
                gridCost[nextId] = lowestCost;
                visited[nextId] = true;
            }

            for(int &i: target){
                if(i==nextId){
                    foundTarget = true;
                    break;
                }
            }
        }
        ///back trace
        std::vector<int> path;
        int curId = nextId;
        while(curId!=src){
            path.emplace_back(curId);
            curId = perdecessor[curId];
        }
        path.emplace_back(src);
        std::reverse(path.begin(),path.end());
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
