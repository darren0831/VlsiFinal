#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include "DisjointSet.hpp"
#include "GraphConstructor.hpp"
#include "GlobalRouter.hpp"
#include "Preprocess.hpp"
#include "DetailRouter.hpp"

int main(int argc, char** argv) {
    setbuf(stdout, nullptr);

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s [-d] <input file>\n", argv[0]);
        return 1;
    }

    bool printToFile = false;
    std::string inputfile;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-d") {
            printToFile = true;
        } else {
            inputfile = argv[i];
        }
    }

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

    Logger stdoutLogger;
    Logger inputLogger("Log/input.log");
    Logger graphLogger("Log/graph.log");
    Logger globalLogger("Log/globalroute.log");
    Logger detailLogger("Log/detailroute.log");

    // Input information
    std::vector<Layer> layers;
    std::vector<Track> tracks;
    std::vector<Bus> buses;
    std::vector<Obstacle> obstacles;
    Rectangle boundary;

    // Graph information
    std::vector<Vertex> vertices;
    std::unordered_map<int, Vertex> vertexMap;
    std::vector<std::vector<Edge>> routingGraph;
    std::vector<Net> nets;

    // Global Result
    std::vector<std::vector<GlobalRoutingPath>> globalResult;

    // Read input file
    stdoutLogger.info("===== Read Input File =====\n");
    {
        Logger& logger = (printToFile) ? inputLogger : stdoutLogger;
        InputReader inputReader(inputfile, logger);
        layers = std::move(inputReader.layers);
        tracks = std::move(inputReader.tracks);
        buses = std::move(inputReader.buses);
        obstacles = std::move(inputReader.obstacles);
        boundary = std::move(inputReader.boundary);
    }

    // Preprocess
    stdoutLogger.info("===== Preprocess =====\n");
    {
        Logger& logger = (printToFile) ? inputLogger : stdoutLogger;
        logger.info("Preprocess\n");
        Preprocess(layers, obstacles, boundary);
    }

    // Construct graph
    stdoutLogger.info("===== Routing Graph Construction =====\n");
    {
        Logger& logger = (printToFile) ? graphLogger : stdoutLogger;
        GraphConstructor graphConstructor(layers, tracks, buses, obstacles, logger);
        vertices = std::move(graphConstructor.vertices);
        vertexMap = std::move(graphConstructor.vertexMap);
        routingGraph = std::move(graphConstructor.routingGraph);
        nets = std::move(graphConstructor.nets);
    }

    // Graph connection check
    {
        Logger& logger = (printToFile) ? graphLogger : stdoutLogger;
        DisjointSet ds(vertices.size());
        for (const auto& edges : routingGraph) {
            for (const auto edge : edges) {
                ds.pack(edge.src, edge.tgt);
            }
        }
        logger.info("Number of connected graphs: %d\n", ds.numGroups());
    }

    // Global route
    stdoutLogger.info("===== Global Route =====\n");
    {
        Logger& logger = (printToFile) ? globalLogger : stdoutLogger;
        GlobalRouter globalRouter(
            layers,
            vertices,
            buses,
            boundary,
            logger);
        globalRouter.globalRoute();
        globalResult = std::move(globalRouter.globalResult);
    }

    // Detail route
    stdoutLogger.info("===== Detail Route =====\n");
    {
        Logger& logger = (printToFile) ? detailLogger : stdoutLogger;
        DetailRouter detailRouter(
            vertices,
            globalResult,
            buses,
            layers,
            vertexMap,
            routingGraph,
            nets,
            logger);
        detailRouter.detailRoute();
    }
    return 0;
}
