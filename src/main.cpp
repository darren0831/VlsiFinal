#include "InputReader.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

int main(int argc, char** argv) {
    Logger stdout_logger("STDOUT");
    InputReader inputReader("example_2.input", stdout_logger);
    auto layers = inputReader.layers;
    auto tracks = inputReader.tracks;
    auto buses = inputReader.buses;
    auto obstacles = inputReader.obstacles;
    // GraphConstructor graphConstructor(layer, tracks, buses, obstacles, stdout_logger);
}
