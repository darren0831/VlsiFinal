#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include "GraphConstructor.hpp"
#include "GlobalRouter.hpp"
#include "Preprocess.hpp"
#include "DetailRouter.hpp"

int main(int argc, char** argv) {
    setbuf(stdout, nullptr);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }

    std::string inputfile = argv[1];

    // Loggers

#ifdef __APPLE__
    mkdir("./Log", 0755);
#endif
#ifdef __unix__
    mkdir("./Log", 0755);
#endif
#ifdef _WIN64
    mkdir("./Log");
#endif

    Logger stdLogger;
    Logger inputLogger("Log/input.log");
    Logger graphLogger("Log/graph.log");

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

    // Global Result
    std::vector<std::vector<GlobalRoutingPath>> globalResult;

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
            buses,
            boundary,
            stdLogger);
        globalRouter.globalRoute();
        globalResult = std::move(globalRouter.globalResult);
    }

    {
        DetailRouter detailRouter(
            vertices,
            globalResult,
            buses,
            layers,
            vertexMap,
            routingGraph,
            nets,
            stdLogger);
        // detailRouter.detailRoute();
        detailRouter.debug();
    }
    return 0;
}
