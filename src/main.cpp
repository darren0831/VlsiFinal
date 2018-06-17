#include <string>
#include "GraphConstructor.hpp"
#include "InputReader.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }
    std::string inputfile = argv[1];
    Logger stdLogger("STDOUT");
    Logger inputLogger("input.log");
    Logger graphLogger("graph.log");
    InputReader inputReader(inputfile, stdLogger);
    auto layers = inputReader.layers;
    auto tracks = inputReader.tracks;
    auto buses = inputReader.buses;
    auto obstacles = inputReader.obstacles;
    GraphConstructor graphConstructor(layers, tracks, buses, obstacles, stdLogger);
    return 0;
}
