#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_

#include <utility>
#include <vector>
#include <string>

class GlobalRoutingPath {
public:
    GlobalRoutingPath(int id, std::string busName, std::pair<int, int> terminal, std::vector<int> gridSequence, std::string sequence) :
        busId(id), busName(busName), terminal(std::move(terminal)), gridSequence(std::move(gridSequence)), topologySequence(std::move(sequence)) {}

    std::string toString() const {
        std::string s;
        for(int i : gridSequence){
            s+=std::to_string(i)+" ";
        }
        return s;
    }

public:
    int busId;
    std::string busName;
    std::pair<int, int> terminal;
    std::vector<int> gridSequence;
    std::string topologySequence;
};

#endif
