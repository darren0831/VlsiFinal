#include "InputReader.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

int main(int argc, char** argv) {
    Logger logger("STDOUT");
    InputReader inputReader("example_1.input", logger);
}
