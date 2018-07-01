#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include "DetailRouter.hpp"
#include "DisjointSet.hpp"
#include "GraphConstructor.hpp"
#include "GlobalRouter.hpp"
#include "OutputGenerator.hpp"
#include "Preprocess.hpp"
#include "DetailRouter.hpp"
#include "NaiveDetailRouter.hpp"

int main(int argc, char** argv) {
    setbuf(stdout, nullptr);

    if (argc != 4 && argc != 5) {
        fprintf(stderr, "usage: %s [-d] <input file> -o <output file>\n", argv[0]);
        return 1;
    }

    bool printToFile = false;
    std::string inputfile;
    std::string outputfile;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-d") {
            printToFile = true;
        } else if (std::string(argv[i]) == "-o") {
            if (i + 1 >= argc) {
                fprintf(stderr, "Invalid command line argument: -o <output file>\n");
                return 1;
            }
            outputfile = argv[i + 1];
            ++i;
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
    std::vector<std::vector<int>> routingGraph;
    std::vector<Edge> routingEdges;
    std::vector<Net> nets;

    // Global Result
    std::vector<std::vector<GlobalRoutingPath>> globalResult;

    // General information
    stdoutLogger.info("Input File: %s\n", inputfile.c_str());
    stdoutLogger.info("Output File: %s\n", outputfile.c_str());

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
        routingGraph = std::move(graphConstructor.routingGraph);
        routingEdges = std::move(graphConstructor.routingEdges);
        nets = std::move(graphConstructor.nets);
    }

    // Graph connection check
    {
        Logger& logger = (printToFile) ? graphLogger : stdoutLogger;
        DisjointSet ds(vertices.size());
        for (const auto& edges : routingGraph) {
            for (const auto edge : edges) {
                const Edge& e = routingEdges[edge];
                ds.pack(e.src, e.tgt);
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
        // globalRouter.globalRoute();
        // globalResult = std::move(globalRouter.globalResult);
    }

    // Detail route
    stdoutLogger.info("===== Detail Route =====\n");
    {
        Logger& logger = (printToFile) ? detailLogger : stdoutLogger;
        NaiveDetailRouter naiveDetailRouter(
            vertices,
            buses,
            layers,
            routingGraph,
            routingEdges,
            nets,
            logger);
        naiveDetailRouter.detailRoute();
    }

    // Output Generate
    stdoutLogger.info("===== Output Generate =====\n");
    {
        Logger& logger = (printToFile) ? detailLogger : stdoutLogger;
        OutputGenerator outputGenerator(
            vertices,
            buses,
            layers,
            nets,
            logger);
        outputGenerator.generateOutput(outputfile);
    }
    return 0;
}
