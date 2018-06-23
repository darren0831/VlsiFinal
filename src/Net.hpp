#ifndef VLSI_FINAL_PROJECT_NET_HPP_
#define VLSI_FINAL_PROJECT_NET_HPP_

#include <vector>

class Net {
public:
    explicit Net(int numBits) :
        numBits(numBits) {
        net = std::vector<std::vector<int>>((unsigned long) numBits);
    }

    Net(const Net& that) {
        net = that.net;
        numBits = that.numBits;
    }

    Net(Net&& that) noexcept {
        net = std::move(that.net);
        numBits = that.numBits;
    }

    Net& operator=(const Net& that) {
        if (this != &that) {
            net = that.net;
            numBits = that.numBits;
        }
        return *this;
    }

    Net& operator=(Net&& that) noexcept {
        if (this != &that) {
            net = std::move(that.net);
            numBits = that.numBits;
        }
        return *this;
    }

    void addTerminal(int binId, int terminalVertexId) {
        net[binId].emplace_back(terminalVertexId);
    }

public:
    std::vector<std::vector<int>> net;
    int numBits;
};

#endif // VLSI_FINAL_PROJECT_NET_HPP_
