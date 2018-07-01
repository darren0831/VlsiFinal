#ifndef VLSI_FINAL_PROJECT_NET_HPP_
#define VLSI_FINAL_PROJECT_NET_HPP_
#include <string>
#include <vector>


class Net {
public:
    explicit Net(int numBits, int numPins) :
        numBits(numBits), numPins(numPins) {
        net = std::vector<std::vector<int>>((unsigned long) numBits);
    }

    Net(const Net& that) {
        net = that.net;
        widths = that.widths;
        numBits = that.numBits;
        numPins = that.numPins;
        detailPath = that.detailPath;
        netName = that.netName;
    }

    Net(Net&& that) noexcept {
        net = std::move(that.net);
        widths = std::move(that.widths);
        numBits = that.numBits;
        numPins = that.numPins;
        detailPath = std::move(that.detailPath);
        netName = std::move(that.netName);
    }

    Net& operator=(const Net& that) {
        if (this != &that) {
            net = that.net;
            widths = that.widths;
            numBits = that.numBits;
            numPins = that.numPins;
            detailPath = that.detailPath;
            netName = that.netName;
        }
        return *this;
    }

    Net& operator=(Net&& that) noexcept {
        if (this != &that) {
            net = std::move(that.net);
            widths = std::move(that.widths);
            numBits = that.numBits;
            numPins = that.numPins;
            detailPath = std::move(that.detailPath);
            netName = std::move(that.netName);
        }
        return *this;
    }

    void addTerminal(int binId, int terminalVertexId) {
        net[binId].emplace_back(terminalVertexId);
    }

public:
    std::vector<std::vector<int>> net;
    std::vector<int> widths;
    std::string netName;
    int numBits;
    int numPins;

public:
    std::vector<std::vector<std::vector<int>>> detailPath;
};

#endif // VLSI_FINAL_PROJECT_NET_HPP_
