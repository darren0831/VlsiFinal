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
        printf("%d\n", bfs(0, 5));
        for(Net &curNet:nets){
            for(int i=0;i<curNet.numBits;i++){

                for(int m=0;m<curNet.net[i].size()-1;m++){
                    for(int n=m+1;n<curNet.net[i].size();n++){
                        bool found = bfs(m,n);
                        if(!found){
                            logger.warning("connect bits by bfs fail\n");
                        }
                    }
                }
            }

        }
    }

private:
    void initialize() {

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
    void implementGlobalGrid() {
        gridWidth = calGridWidth();
        xGridCount = ceil(boundary.up.x/gridWidth);
        yGridCount = ceil(boundary.up.y/gridWidth);
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

    bool bfs(int src, int tgt){
        std::queue<int> vertexQueue;
        bool isvisited[vertices.size()];
        for(bool& it : isvisited) it = false;
            vertexQueue.push(src);
        while(!vertexQueue.empty()){
            int cur = vertexQueue.front();
            vertexQueue.pop();
            if(cur==tgt) return true;
            for(Vertex &neighbor : routingGraph[cur]){
                if(isvisited[neighbor.id]==false){
                    isvisited[neighbor.id]=true;
                    vertexQueue.push(neighbor.id);
                }
            }
        }
        return false;
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
            if(layers[layer].isHorizontal()) {
                return AgridID > BgridID ? 'L' : 'R';
            }else if(layers[layer].isVertical()){
                return AgridID > BgridID ? 'F' : 'B';
            }
        }
        
    }

private:
    std::vector<Vertex>& vertices;
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;
    std::vector<Net>& nets;
    std::vector<Bus>& buses;
    std::vector<std::vector<int>> globalGraph;
    std::vector<GlobalEdge> globalEdges;
    Rectangle& boundary;

private:
    Logger& logger;
    double gridWidth;
    int xGridCount, yGridCount;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
