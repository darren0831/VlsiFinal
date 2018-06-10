#include "InputReader.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"

int main(int argc, char** argv) {
    Logger logger("InputInfo");
    InputReader inputReader("example_2.input", logger);
}
