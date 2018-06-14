#include "InputReader.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

int main(int argc, char** argv) {
    Logger logger("InputInfo");
    InputsReader inputReader("example_2.input", logger);
    auto layers = inputReader.layers;
    auto tracks = inputReader.tracks;
    auto buses = inputReader.buses;
    auto obstacles = inputReader.obstacles;
    GraphConstructor graphConstructor(layer, tracks, buses, obstacles);
}
