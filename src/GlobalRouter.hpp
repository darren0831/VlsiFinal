#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <unordered_map>
#include <vector>
#include <queue>
#include "Vertex.hpp"

class GlobalRouter {
public:
    GlobalRouter(std::vector<Vertex>& vertices,
                 std::unordered_map<int, Vertex>& vertexMap,
                 std::vector<std::vector<Vertex>>& routingGraph,
                 std::vector<Net>& nets,
                 Logger& logger) :
        vertices(vertices), vertexMap(vertexMap), routingGraph(routingGraph), nets(nets), logger(logger) {}

    void globalRoute() {
        printf("%d\n", bfs(0, 5));
    }

private:
    void initialize() {

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

private:
    Logger& logger;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
