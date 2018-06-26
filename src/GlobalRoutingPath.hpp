#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_

#include <utility>
#include <vector>
#include <string>

class GlobalRoutingPath {
public:
    GlobalRoutingPath(int id, std::pair<int, int> terminal, std::vector<int> gridSequence, std::string sequence) :
        busId(id), terminal(std::move(terminal)), gridSequence(std::move(gridSequence)), topologySequence(std::move(sequence)) {}

    std::string toString() const {
        return topologySequence;
    }

private:
    int busId;
    std::pair<int, int> terminal;
    std::vector<int> gridSequence;
    std::string topologySequence;
};

#endif
