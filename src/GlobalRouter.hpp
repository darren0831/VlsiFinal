#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <unordered_map>
#include <vector>
#include "Vertex.hpp"

class GlobalRouter {
private:
    class CombinedVector {
    public:
    private:
        std::unordered_set<int> vertexSet;
    };

public:
    GlobalRouter(std::vector<Vertex>& vertices,
                 std::unordered_map<int, Vertex>& vertexMap,
                 std::vector<std::vector<Vertex>>& routingMap) :
        vertices(vertices), vertexMap(vertexMap), routingGraph(routingGraph) {}

    void globalRoute() {

    }

private:
    void initialize() {

    }

private:
    std::vector<Vertex>& vertices;
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;

private:

};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
