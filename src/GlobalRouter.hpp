#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <unordered_map>
#include <vector>
#include <queue>
#include "Vertex.hpp"

class GlobalRouter {
private:
    class GlobalGrid {
    public:
        GlobalGrid(int width,std::vector<int>& vertices):width(width),vertices(vertices) {}
        int width;
        std::vector<int> vertices;
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

private:
    std::vector<Vertex>& vertices;
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;
    std::vector<Net>& nets;
    std::vector<Bus>& buses;

private:
    Logger& logger;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
