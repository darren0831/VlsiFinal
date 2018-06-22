#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <unordered_map>
#include <vector>
#include <queue>
#include <cmath>
#include "Vertex.hpp"
#include "Layer.hpp"

class GlobalRouter {
private:
    class GlobalEdge {
    public:
        GlobalEdge(int id,int src,int tgt):id(id),src(src),tgt(tgt) {
            cost = 0;
            historical_cost = 0;
            direction = 0;
            maxWidth = 0;
        }
        double getCost() {return cost;}
        void setCost(double cost) {this->cost = cost;}
        double getHistoricalCost() {return historical_cost;}
        void setHistoricalCost(double historical_cost) {this->historical_cost = historical_cost;}

        int edgeRequest(int count, double width) {

        }
        void edgeRecover(int operId) {

        }
        int edgeCount(double querywidth) {
            if(layer==-1) return
        }
        void addVertexToEdge(Vertex v) {
            vertices[v.id] = ceil(v.track.width);
            maxWidth = std::max(v.track.width,maxWidth);
        }
    public:
        int src;
        int tgt;
        int id;
        int layer;
    private:
        double cost;
        double historical_cost;
        std::unordered_map<int,int> vertices;
        int maxWidth;


    };
public:
    GlobalRouter(std::vector<Layer>& layers,
       std::vector<Vertex>& vertices,
       std::unordered_map<int, Vertex>& vertexMap,
       std::vector<std::vector<Vertex>>& routingGraph,
       std::vector<Net>& nets,
       std::vector<Bus>& buses,
       Rectangle& boundary,
       Logger& logger) :
    layers(layers), vertices(vertices), vertexMap(vertexMap), routingGraph(routingGraph), nets(nets), buses(buses), boundary(boundary), logger(logger) {}

    void globalRoute() {
        initialize();
        prepareNets();
        doRouting();
    }

private:
    void initialize() {
        constructGlobalEdge();
        bindVertexToEdge();
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

    double calGridWidth() {
        std::vector<int> medians;
        for(int i=0;i<(int)buses.size();i++){
            std::vector<int> s = buses[i].widths;
            std::sort(s.begin(),s.end());
            medians.emplace_back(buses[i].widths[buses[i].widths.size()/2]);
        }
        std::sort(medians.begin(),medians.end());
        return medians[medians.size()/2];
    }
    void constructGlobalEdge() {
        gridWidth = calGridWidth();
        xGridCount = ceil(boundary.ur.x/gridWidth);
        yGridCount = ceil(boundary.ur.y/gridWidth);
        int edgeId=0;
        globalGraph = std::vector<std::vector<int>>(xGridCount*yGridCount*(int)layers.size());
        for(auto& v: globalGraph){
            v = std::vector<int>(6);
        }
        for(int k=0;k<(int)layers.size();k++) {
            for(int i=0;i<xGridCount-1;i++) {
                for(int j=0;j<yGridCount-1;j++) {
                    if(k!=(int)layers.size()-1)
                    {
                        GlobalEdge via(edgeId,(i*xGridCount+j)*k,(i*xGridCount+j)*(k+1));
                        via.layer = -1;
                        globalEdges.emplace_back(via);
                        globalGraph[(i*xGridCount+j)*k].emplace_back(edgeId);
                        globalGraph[(i*xGridCount+j)*(k+1)].emplace_back(edgeId);
                        ++edgeId;
                    }
                    if(layers[k].isHorizontal())
                    {
                        GlobalEdge lr(edgeId,(i*xGridCount+j)*k,(i*xGridCount+(j+1))*k);
                        lr.layer = k;
                        globalEdges.emplace_back(lr);
                        globalGraph[(i*xGridCount+j)*k].emplace_back(edgeId);
                        globalGraph[(i*xGridCount+(j+1))*k].emplace_back(edgeId);
                        ++edgeId;
                    }else if(layers[k].isVertical())
                    {
                        GlobalEdge fb(edgeId,(i*xGridCount+j)*k,((i+1)*xGridCount+j)*k);
                        fb.layer = k;
                        globalEdges.emplace_back(fb);
                        globalGraph[(i*xGridCount+j)*k].emplace_back(edgeId);
                        globalGraph[((i+1)*xGridCount+j)*k].emplace_back(edgeId);
                        ++edgeId;
                    }
                }
            }
        }
    }
    void bindVertexToEdge(){
        for(int i=0;i<(int)vertices.size();i++){
            int layer = vertices[i].track.layer;
            Point from = vertices[i].track.rect.ll;
            Point to = vertices[i].track.rect.ur;
            int fromGridID = coordToGridId(from, layer);
            int toGridID = coordToGridId(to, layer);
            char vertexDir = getDirection(fromGridID,layer,toGridID,layer);
            if(layers[layer].isHorizontal()) {
                for(int j=fromGridID;j<toGridID;j++){
                    for(int k=0;k<(int)globalGraph[j].size();k++){
                        if(getDirection(globalEdge[globalGraph[j][i]].src,globalEdge[globalGraph[j][i]].tgt)==vertexDir){
                            globalEdge[globalGraph[j][i]].addVertexToEdge(vertices[i]);
                            globalEdge[globalGraph[j+1][i]].addVertexToEdge(vertices[i]);
                        }
                    }
                }
            }else if(layers[layer].isVertical()){
                for(int j=0;j<(toGridID-fromGridID)/xGridCount;j++){
                    for(int k=0;k<(int)globalGraph[j].size();k++){
                        if(getDirection(globalEdge[globalGraph[j*xGridCount+fromGridID][i]].src,globalEdge[globalGraph[j*xGridCount+fromGridID][i]].tgt)==vertexDir){
                            globalEdge[globalGraph[j*xGridCount+fromGridID][i]].addVertexToEdge(vertices[i]);
                            globalEdge[globalGraph[(j+1)*xGridCount+fromGridID][i]].addVertexToEdge(vertices[i]);
                        }
                    }
                }
            }
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

    int coordToGridId(Point p,int layer){
        int x = ceil(p.x/gridWidth);
        int y = ceil(p.y/gridWidth);
        return (x*xGridCount+y)*layer;
    }
    char getDirection(int AgridID, int Alayer, int BgridID, int Blayer) {
        if(Alayer!=Blayer){
            return AgridID > BgridID ? 'U' : 'D';
        }
        else {
            int layer = Alayer;
            if(layers[layer].isHorizontal()) {
                return AgridID > BgridID ? 'L' : 'R';
            }else if(layers[layer].isVertical()){
                return AgridID > BgridID ? 'F' : 'B';
            }
        }
    }

private:
    std::vector<Layer>& layers;
    std::vector<Vertex>& vertices;
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;
    std::vector<Net>& nets;
    std::vector<Bus>& buses;

private:
    std::vector<std::vector<int>> globalGraph;
    std::vector<GlobalEdge> globalEdges;
    Rectangle& boundary;

private:
    std::vector<std::vector<int>> globalNets;
    std::vector<std::vector<int>> globalNetWidths;

private:
    Logger& logger;
    double gridWidth;
    int xGridCount, yGridCount;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
