#ifndef VLSI_FINAL_PROJECT_NET_HPP_
#define VLSI_FINAL_PROJECT_NET_HPP_

#include <vector>

class Net {
public:
    Net(int numBins) :
        numBins(numBins) {
        net = std::vector<std::vector<int>>(numBins);
    }

    void addTerminal(int binId, int terminalVertexId) {
        net[binId].emplace_back(terminalVertexId);
    }

public:
    std::vector<std::vector<int>> net;
    int numBins;
};
#endif // VLSI_FINAL_PROJECT_NET_HPP_
