#include <string>
#include "GraphConstructor.hpp"
#include "GlobalRouter.hpp"
#include "Preprocess.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }
    std::string inputfile = argv[1];
    Logger stdLogger;
    Logger inputLogger("input.log");
    Logger graphLogger("graph.log");
    Logger globalRoutingLogger("global.log");
    InputReader inputReader(inputfile, stdLogger);
    auto layers = inputReader.layers;
    auto tracks = inputReader.tracks;
    auto buses = inputReader.buses;
    auto obstacles = inputReader.obstacles;
    Preprocess(layers,obstacles);
    GraphConstructor graphConstructor(layers, tracks, buses, obstacles, stdLogger);
    auto vertices = graphConstructor.vertices;
    auto vertexMap = graphConstructor.vertexMap;
    auto routingGraph = graphConstructor.routingGraph;
    GlobalRouter globalRouter(vertices, vertexMap, routingGraph, globalRoutingLogger);
    return 0;
}
