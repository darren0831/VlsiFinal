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

    // Loggers
    Logger stdLogger;
    Logger inputLogger("input.log");
    Logger graphLogger("graph.log");
    Logger globalRoutingLogger("global.log");

    // Input information
    std::vector<Layer> layers;
    std::vector<Track> tracks;
    std::vector<Bus> buses;
    std::vector<Obstacle> obstacles;
    Rectangle boundary;

    // Graph information
    std::vector<Vertex> vertices;
    std::unordered_map<int, Vertex> vertexMap;
    std::vector<std::vector<Vertex>> routingGraph;
    std::vector<Net> nets;

    // Read input file
    {
        InputReader inputReader(inputfile, stdLogger);
        layers = std::move(inputReader.layers);
        tracks = std::move(inputReader.tracks);
        buses = std::move(inputReader.buses);
        obstacles = std::move(inputReader.obstacles);
        boundary = std::move(inputReader.boundary);
    }

    // Preprocess
    {
        Preprocess(layers, obstacles, boundary);
    }

    // Construct graph
    {
        GraphConstructor graphConstructor(layers, tracks, buses, obstacles, stdLogger);
        vertices = std::move(graphConstructor.vertices);
        vertexMap = std::move(graphConstructor.vertexMap);
        routingGraph = std::move(graphConstructor.routingGraph);
        nets = std::move(graphConstructor.nets);
    }

    // Global route
    {
        GlobalRouter globalRouter(
            layers,
            vertices,
            vertexMap,
            routingGraph,
            nets,
            buses,
            boundary,
            stdLogger);
        globalRouter.globalRoute();
    }
    return 0;
}
