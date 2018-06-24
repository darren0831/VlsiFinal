#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTING_PATH_HPP_

#include <utility>
#include <string>


class GlobalRoutingPath{
public:
    GlobalRoutingPath(int id,std::pair<int,int> terminal, std::string sequence) :
        busId(id),terminal(terminal),sequence(sequence){}

private:
    int busId;
    std::pair<int,int> terminal;
    std::string sequence;


};

#endif
