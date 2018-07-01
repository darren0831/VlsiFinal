#ifndef VLSI_FINAL_PROJECT_BIRD_DETAIL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_BIRD_DETAIL_ROUTER_HPP_

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <deque>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "Bit.hpp"
#include "Bus.hpp"
#include "Edge.hpp"
#include "FenwickTree.hpp"
#include "GlobalRoutingPath.hpp"
#include "GraphConstructor.hpp"
#include "InputReader.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Net.hpp"
#include "Obstacle.hpp"
#include "Pin.hpp"
#include "Point.hpp"
#include "Rectangle.hpp"
#include "SegmentMap.hpp"
#include "Track.hpp"
#include "Vertex.hpp"
#include "VertexUsage.hpp"

class BirdDetailRouter {
private:
    struct RoutingNode {
        std::vector<int> node;
        std::vector<int> fromNode;
        double cost;

        RoutingNode(double cost) :
            cost(cost) {
            node = std::vector<int>();
            fromNode = std::vector<int>();
        }

        bool operator<(const RoutingNode& that) const {
            if (fabs(cost - that.cost) > 1e-6) {
                return cost < that.cost;
            } else {
                return node[0] < that.node[0] || (node[0] == that.node[0] && fromNode[0] < that.fromNode[0]);
            }
        }
    };

public:
    BirdDetailRouter(std::vector<Vertex>& vertices,
        std::vector<Bus>& buses,
        std::vector<Layer>& layers,
        std::vector<std::vector<int>>& routingGraph,
        std::vector<Edge>& routingEdges,
        std::vector<Net>& nets,
        Logger& logger):
        vertices(vertices), buses(buses), layers(layers), routingGraph(routingGraph), routingEdges(routingEdges), nets(nets), logger(logger) {
            initialize();
        }

    void initialize() {
        usage = std::vector<VertexUsage>(vertices.size());
        visited = std::vector<bool>(vertices.size());
        for (int i = 0; i < (int) usage.size(); ++i) {
            usage[i] = VertexUsage(vertices[i].track.length() + 8);
            usage[i].initialize();
            visited[i] = false;
        }
        costs = std::vector<double>(vertices.size());
        for (int i = 0; i < (int) costs.size(); ++i) {
            costs[i] = 1e10;
        }
    }

    void detailRoute() {
        std::stack<int> routingCandidates;
        std::vector<int> routingOrder(nets.size());
        for (int i = 0; i < (int) nets.size(); ++i) {
            routingOrder[i] = i;
        }
        std::sort(routingOrder.begin(), routingOrder.end(), [&](int a, int b) {
            const int bitA = buses[a].numBits;
            const int bitB = buses[b].numBits;
            return bitA < bitB;
        });
        for (int i = 0; i < (int) routingOrder.size(); ++i) {
            routingCandidates.push(routingOrder[i]);
        }
        initialize();
        while (!routingCandidates.empty()) {
            int next = routingCandidates.top();
            routingCandidates.pop();
            Net& net = nets[next];
            logger.info("Route bus %s\n", buses[next].name.c_str());
            routeSingleBus(net);
        }
    }

    void routeSingleBus(Net& net) {
        // initialize
        net.detailPath.clear();
        // net target, a vector for each bit
        std::vector<std::vector<int>> targets(net.numBits);
        // initialize
        for (int i = 0; i < net.numBits; ++i) {
            // pin 0 of bit i
            targets[i].emplace_back(net.net[i][0]);
        }
        // route pin 1 to pin 0 (for each bit), then
        // route pin 2 to pin 0, 1 (for each bit), then
        // route pin 3 to pin 0, 1, 2 (for each bit), and so on
        for (int i = 1; i < net.numPins; ++i) {
            std::vector<int> sources;
            for (int j = 0; j < net.numBits; ++j) {
                // net[j][i]: pin i of bit j
                sources.emplace_back(net.net[j][i]);
            }
            logger.info("Route Pin %d\n", i);
            routeSingleTwoPinNet(net, sources, targets);
        }
    }

    void routeSingleTwoPinNet(Net& net, const std::vector<int>& src, const std::vector<std::vector<int>>& tgt) {
        std::priority_queue<RoutingNode> q;
        std::vector<std::vector<int>> predecessor = std::vector<std::vector<int>>(net.numBits);
        // topology de-queue
        std::deque<char> topology;
        // end point record
        std::vector<int> ends(vertices.size());
        for (int i = 0; i < (int) ends.size(); ++i) {
            ends[i] = -1;
        }
        for (int b = 0; b < (int) tgt.size(); ++b) {
            for (int v : tgt[b]) {
                ends[v] = b;
            }
        }
        RoutingNode first(0.0);
        first.node = src;
        first.fromNode = std::vector<int>(src.size(), -1);
        q.push(first);
        while (!q.empty()) {
            RoutingNode top = q.top();
            q.pop();
            std::queue<int> cand;
            for (int i = 0; i < net.numBits; ++i) {
                if (ends[top.node[i]] != i) {
                    cand.push(i);
                } else {
                    logger.info("Bit %d completed!\n");
                }
            }
            while (!cand.empty()) {
                int pt = cand.front();
                cand.pop();
                int nodeId = top.node[pt];
                for (int edgeId : routingEdges[])
            }
        }
    }

private:
    bool isTopologyMatched(const std::deque<char>& topology, char toCheck) const {
        if (toCheck != 'U' && toCheck != 'D' &&
            toCheck != 'L' && toCheck != 'R' &&
            toCheck != 'F' && toCheck != 'B' &&
            toCheck != ' ') {
            logger.error("Fatal: Invalid topology type: %c\n", toCheck);
            return false;
        }
        if (topology.empty() || toCheck == ' ') {
            return true;
        } else {
            return topology.back() == toCheck;
        }
    }

private:
    std::vector<Vertex>& vertices;
    std::vector<Bus>& buses;
    std::vector<Layer>& layers;

private:
    std::vector<std::vector<int>>& routingGraph;
    std::vector<Edge>& routingEdges;
    std::vector<Net>& nets;
    Logger& logger;

private:
    std::vector<VertexUsage> usage;
    std::vector<bool> visited;
    std::vector<double> costs;
};

#endif // VLSI_FINAL_PROJECT_BIRD_DETAIL_ROUTER_HPP_
