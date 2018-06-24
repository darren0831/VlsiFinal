#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Bit.hpp"
#include "Bus.hpp"
#include "Edge.hpp"
#include "FenwickTree.hpp"
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

class GlobalRouter {
private:
    constexpr static double EXPECTED_GRID_COUNT = 150.0;
    constexpr static double EDGE_COST_ALPHA = 1000.0;

private:
    class GlobalEdge {
    public:
        GlobalEdge(int id_, int src_, int tgt_, int maxVertexWidth) :
            id(id_), src(src_), tgt(tgt_), maxWidth(maxVertexWidth) {
            historical_cost = 0;
            ft = FenwickTree(maxWidth);
        }

        int getTarget(int source) const {
            return (source == src) ? tgt : src;
        }

        double getHistoricalCost() {
            return historical_cost;
        }

        void setHistoricalCost(double historical_cost) {
            this->historical_cost = historical_cost;
        }

        int edgeRequest(int count, int width) {
            std::vector<std::pair<int,int>> remove = ft.remove(width,count);
            return insertOper(remove);
        }

        void edgeRecover(int operId) {
            std::vector<std::pair<int,int>> recover = operation[operId];
            isOperUsed[operId]=0;
            for (auto& i : recover) {
                ft.insert(i.first, i.second);
            }
        }

        int edgeCount(int querywidth) {
            if (layer == -1) { return std::numeric_limits<int>::max(); }
            return ft.query(querywidth);
        }

        void addVertexToEdge(Vertex v) {
            vertices[v.id] = (int) ceil(v.track.width);
            ft.insert((int) v.track.width, 1);
        }

        int insertOper(std::vector<std::pair<int,int>> p){
            for(int i=0;i<(int)operation.size();i++){
                if(isOperUsed[i]==0){
                    operation[i]=p;
                    isOperUsed[i]=1;
                    return i;
                }
            }
            isOperUsed.emplace_back(1);
            operation.emplace_back(p);
            return (int) operation.size()-1;
        }
        void pushRequestBusId(int id){
            requestBusId.emplace_back(id);
        }
        void popRequestBusId(int id) {
            auto index = std::find(requestBusId.begin(),requestBusId.end(),id);
            if(index!= requestBusId.end())
                requestBusId.erase(index);
        }

    public:
        int id;
        int src;
        int tgt;
        int layer;
        std::vector<int> requestBusId;
    private:
        int maxWidth;
        double historical_cost;
        FenwickTree ft;
        std::vector<std::vector<std::pair<int,int>>> operation;
        std::vector<int> isOperUsed;
        std::unordered_map<int, int> vertices;
        
    };

private:
    struct VisitNode {
        int nodeId;
        int fromNode;
        int fromEdge;
        double cost;

        VisitNode(int nodeId, int fromNode, int fromEdge, double cost) :
            nodeId(nodeId), fromNode(fromNode), fromEdge(fromEdge), cost(cost) {}

        bool operator<(const VisitNode& that) const {
            return cost < that.cost || (fabs(cost - that.cost) < 1e-6 && nodeId < that.nodeId);
        }
    };

public:
    GlobalRouter(std::vector<Layer>& layers,
                 std::vector<Vertex>& vertices,
                 std::vector<Bus>& buses,
                 Rectangle& boundary,
                 Logger& logger) :
        layers(layers), vertices(vertices), buses(buses), boundary(boundary), logger(logger) {
        failCount = std::vector<int>(buses.size());
    }

    void globalRoute() {
        logger.info("Initialize\n");
        initialize();
        logger.info("Prepare nets\n");
        prepareNets();
        logger.info("Do global route\n");
        doGlobalRouting();
    }

private:
    void initialize() {
        logger.info("Calculate Max Vertex Width\n");
        calMaxVertexWidth();
        logger.info("Construct global edges\n");
        constructGlobalEdge();
        logger.info("Bind vertex to edges\n");
        bindVertexToEdge();
        printGlobalEdge();
    }

    void printGlobalEdge(){
        Logger logs("Log/GlobalGraph.log");
        for(int i=0;i<(int)globalGraph.size();i++){
            logs.show("%d: <",i);
            for(int j=0;j<(int)globalGraph[i].size();j++){
                logs.show("%d ",globalEdges[globalGraph[i][j]].getTarget(i));
            }
            logs.show(">\n");
        }
    }

    void prepareNets() {
        globalNets = std::vector<std::vector<int>>();
        globalNetWidths = std::vector<std::vector<int>>();
        for (const Bus& bus : buses) {
            std::vector<int> width;
            for (int i = 0; i < (int) bus.widths.size(); ++i) {
                width.emplace_back(bus.widths[i]);
            }
            std::vector<int> grids;
            for (int i = 0; i < bus.numPins; ++i) {
                std::vector<int> pinGrids;
                for (int j = 0; j < bus.numBits; ++j) {
                    const Pin& pin = bus.bits[j].pins[i];
                    Point midPoint = pin.rect.midPoint();
                    int gridId = coordToGridId(midPoint, pin.layer);
                    pinGrids.emplace_back(gridId);
                }
                std::unordered_map<int, int> countMap;
                for (int v : pinGrids) {
                    countMap[v]++;
                }
                int maxid = -1;
                int maxv = 0;
                for (const auto& vp : countMap) {
                    if (vp.second > maxv) {
                        maxv = vp.second;
                        maxid = vp.first;
                    }
                }
                grids.emplace_back(maxid);
            }
            globalNets.emplace_back(std::move(grids));
            globalNetWidths.emplace_back(std::move(width));
        }
    }

    void doGlobalRouting() {
        netOperations = std::vector<std::vector<std::pair<int, int>>>(globalNets.size());
        int routingOrderList[globalNets.size()];
        for (int i = 0; i < (int) globalNets.size(); ++i) {
            routingOrderList[i] = i;
        }
        std::sort(routingOrderList, routingOrderList + globalNets.size(), [&](int a, int b) {
            const std::vector<int>& va = globalNets[a];
            const std::vector<int>& vb = globalNets[b];
            const int bitA = buses[a].numBits;
            const int bitB = buses[b].numBits;
            const auto termA = (int) va.size();
            const auto termB = (int) vb.size();
            if (bitA != bitB) {
                return bitA > bitB;
            } else if (termA != termB) {
                return termA > termB;
            } else {
                return a < b;
            }
        });
        std::stack<int> stack;
        for (int i=(int)globalNets.size()-1;i>=0;--i){
            stack.push(routingOrderList[i]);
        }
        while(!stack.empty()){
            int idx = stack.top();
            stack.pop();
            logger.info("Routing bus %s\n", buses[idx].name.c_str());
            routeSingleNet(idx, globalNets[idx], globalNetWidths[idx], buses[idx].numBits, stack);
        }
    }

    void calMaxVertexWidth() {
        maxVertexWidth = 0;
        for (auto& v: vertices) {
            maxVertexWidth = std::max(maxVertexWidth, (int) v.track.width);
        }
    }

    double calGridWidth() {
        std::vector<int> medians;
        for (int i = 0; i < (int) buses.size(); i++) {
            std::vector<int> s = buses[i].widths;
            std::sort(s.begin(), s.end());
            medians.emplace_back(buses[i].widths[buses[i].widths.size() / 2]);
        }
        std::sort(medians.begin(), medians.end());
        return medians[medians.size() / 2];
    }

    void constructGlobalEdge() {
        gridWidth = calGridWidth();
        double shortBoundary = std::min(boundary.ur.x,boundary.ur.y);
        double expectGridWidth = shortBoundary/EXPECTED_GRID_COUNT;
        gridWidth = std::max(expectGridWidth,gridWidth);
        xGridCount = (int) ceil(boundary.ur.x/gridWidth);
        yGridCount = (int) ceil(boundary.ur.y/gridWidth);

        int edgeId=0;
        globalGraph = std::vector<std::vector<int>>(xGridCount * yGridCount * layers.size());
        logger.info("gridWidth: %lf\n", gridWidth);
        logger.info("Grid: %d * %d\n", xGridCount, yGridCount);
        for(int k=0;k<(int)layers.size();k++) {
            for(int i=0;i<yGridCount;i++) {
                for(int j=0;j<xGridCount;j++) {
                    if(k<(int)layers.size()-1)
                    {
                        GlobalEdge via(edgeId,(i*xGridCount+j) + k*xGridCount*yGridCount,(i*xGridCount+j) + (k+1)*xGridCount*yGridCount,maxVertexWidth);
                        via.layer = -1;
                        globalEdges.emplace_back(via);
                        globalGraph[(i * xGridCount + j) + k*xGridCount*yGridCount].emplace_back(edgeId);
                        globalGraph[(i * xGridCount + j) + (k+1)*xGridCount*yGridCount].emplace_back(edgeId);
                        ++edgeId;
                    }
                    if(layers[k].isHorizontal())
                    {
                        if(j<xGridCount-1){
                            GlobalEdge lr(edgeId,(i*xGridCount+j) + k*xGridCount*yGridCount,(i*xGridCount+(j+1)) + k*xGridCount*yGridCount,maxVertexWidth);
                            lr.layer = k;
                            globalEdges.emplace_back(lr);
                            globalGraph[(i * xGridCount + j) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            globalGraph[(i * xGridCount + (j+1)) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            ++edgeId;
                        }
                    }else {
                        if(i<yGridCount-1){
                            GlobalEdge fb(edgeId,(i*xGridCount+j) + k*xGridCount*yGridCount,((i+1)*xGridCount+j) + k*xGridCount*yGridCount,maxVertexWidth);
                            fb.layer = k;
                            globalEdges.emplace_back(fb);
                            globalGraph[(i*xGridCount+j) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            globalGraph[((i+1)*xGridCount+j) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            ++edgeId;
                        }
                    }
                }
            }
        }
        logger.info("Global Graph vertex size %lu\n", globalGraph.size());
        logger.info("Global Graph edge size %lu\n", globalEdges.size());
    }

    void bindVertexToEdge() {
        for (const Vertex& v : vertices) {
            int layer = v.track.layer;
            Point from = v.track.terminal[0];
            Point to = v.track.terminal[1];
            int fromGridId = coordToGridId(from, layer);
            int toGridId = coordToGridId(to, layer);
            char vertexDir = getDirection(fromGridId, toGridId);
            if (layers[layer].isHorizontal()) {
                for (int i = fromGridId; i < toGridId; ++i) {
                    for (int j : globalGraph[i]) {
                        if (getDirection(globalEdges[j].src, globalEdges[j].tgt)) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                    for (int j : globalGraph[i + 1]) {
                        if (getDirection(globalEdges[j].src, globalEdges[j].tgt)) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                }
            } else {
                for (int i = fromGridId; i < toGridId - xGridCount; i += xGridCount) {
                    for (int j : globalGraph[i]) {
                        if (getDirection(globalEdges[j].src, globalEdges[j].tgt) == vertexDir) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                    for (int j : globalGraph[i + xGridCount]) {
                        if (getDirection(globalEdges[j].src, globalEdges[j].tgt) == vertexDir) {
                            globalEdges[j].addVertexToEdge(v);

                        }
                    }
                }
            }
        }
    }

    bool routeSingleNet(int id, const std::vector<int>& net, const std::vector<int>& widths, const int numBits, std::stack<int>& stack) {
        std::vector<int> tgt;
        tgt.emplace_back(net.at(0));
        for (int i = 1; i < (int) net.size(); ++i) {
            logger.info("  - Routing pins %d / %d\n", i, (int) net.size() - 1);
            int src = net[i];
            if (std::count(tgt.begin(), tgt.end(), src) > 0) {
                logger.info("      Omitted! Source and target are in the same grid\n");
                continue;
            }
            auto result = routeSinglePath(id, src, tgt, widths, numBits);
            if (result[0].first != -1) {
                for (const auto& v : result) {
                    tgt.emplace_back(v.first);
                }
            } else {
                if(failCount[id]>=1) {
                    logger.warning("    > What a Terrible Fail\n");
                    return false;
                }
                failCount[id]++;
                logger.warning("    > Failed! Do rip-up re-route\n");
                if(result[0].second == -1) {
                    logger.warning("    > What a Significantly Terrible Fail\n");
                    return false;
                }
                GlobalEdge& edge = globalEdges[result[0].second];
                logger.info("        Recover bus name: %s\n",buses[id].name.c_str());
                for (const auto& p : netOperations[id]){    
                    globalEdges[p.first].edgeRecover(p.second);
                    globalEdges[p.first].popRequestBusId(id);
                }
                netOperations[id].clear();
                for(auto it = edge.requestBusId.rbegin();it!=edge.requestBusId.rend();++it){
                    logger.info("        Recover bus name: %s\n",buses[*it].name.c_str());
                    for (const auto& p : netOperations[*it]){
                        globalEdges[p.first].edgeRecover(p.second);
                        globalEdges[p.first].popRequestBusId(*it);
                    }
                    netOperations[*it].clear();
                    stack.push(*it);
                }
                stack.push(id);
            }
        }
        return true;
    }


    std::vector<std::pair<int, int>> routeSinglePath(const int id, const int src, const std::vector<int>& target, const std::vector<int>& width, const int bitCount) {
        std::priority_queue<VisitNode> pQ;
        bool visited[globalGraph.size()];
        bool targetVertex[globalGraph.size()];
        std::vector<double> gridCost(globalGraph.size());
        std::vector<double> edgeHistoricalCost(globalEdges.size());
        std::vector<int> predecessor(globalGraph.size());
        std::vector<int> preEdges(globalGraph.size());
        for (int i = 0; i < (int) globalGraph.size(); ++i) {
            visited[i] = false;
            targetVertex[i] = false;
            gridCost[i] = 1e12;
        }
        for (int v : target) {
            targetVertex[v] = true;
        }
        bool found = false;
        int finalVertex = -1;
        int firstEdgeId = -1;
        gridCost[src] = 0;
        pQ.push(VisitNode(src, -1, -1, 0));
        while (!pQ.empty()) {
            VisitNode top = pQ.top();
            pQ.pop();
            int node = top.nodeId;
            if (visited[node]) {
                continue;
            }
            visited[node] = true;
            predecessor[node] = top.fromNode;
            preEdges[node] = top.fromEdge;
            if (targetVertex[node]) {
                found = true;
                finalVertex = top.nodeId;
                break;
            }
            for (int edgeId : globalGraph[node]) {
                GlobalEdge& edge = globalEdges[edgeId];
                int out = (node == edge.tgt) ? edge.src : edge.tgt;
                if (visited[out]) {
                    continue;
                }
                int edgeNotEnough = 0;
                if (edge.layer != -1) {
                    edgeNotEnough = bitCount - edge.edgeCount(width[edge.layer]);
                    edgeNotEnough = std::max(0, edgeNotEnough);
                }
                double edgeCountCost = 0;
                if (edgeNotEnough > 15) {
                    if(firstEdgeId == -1 && !edge.requestBusId.empty())
                        firstEdgeId = edgeId;
                    continue;
                } else {
                    edgeCountCost = EDGE_COST_ALPHA * edgeNotEnough;
                }
                if (gridCost[node] + globalEdges[edgeId].getHistoricalCost() + edgeCountCost < gridCost[out]) {
                    gridCost[out] = gridCost[node] + globalEdges[edgeId].getHistoricalCost() + edgeCountCost;
                    edgeHistoricalCost[edgeId] = globalEdges[edgeId].getHistoricalCost() + edgeCountCost;
                    pQ.push(VisitNode(out, node, edgeId, gridCost[out]));
                }
            }
        }
        logger.info("      Path finding finished\n");
        if (!found || finalVertex == -1) {
            std::vector<std::pair<int, int>> temp;
            temp.emplace_back(std::make_pair(-1,firstEdgeId));
            return temp;
        }
        std::vector<std::pair<int, int>> path;
        while (finalVertex != -1) {
            path.emplace_back(std::make_pair(finalVertex, preEdges[finalVertex]));
            finalVertex = predecessor[finalVertex];
        }
        std::reverse(path.begin(), path.end());
        for (const auto& v : path) {
            if (v.second != -1) {
                GlobalEdge& edge = globalEdges[v.second];
                if (edge.layer != -1) {
                    int operationId = edge.edgeRequest(bitCount, width[edge.layer]);
                    edge.pushRequestBusId(id);
                    edge.setHistoricalCost(edgeHistoricalCost[v.second]);
                    netOperations[id].emplace_back(std::make_pair(v.second, operationId));
                }
            }
        }
        logger.info("      Back trace finished\n");
//        for (const auto& v : path) {
//            logger.info("  - node %d, from edge %d\n", v.first, v.second);
//        }
        return path;
    }

    int coordToGridId(Point p, int layer) {
        auto x = (int) floor(p.x / gridWidth);
        auto y = (int) floor(p.y / gridWidth);
        return (y * xGridCount + x) + layer * xGridCount * yGridCount;
    }

    char getDirection(int AgridID, int BgridID) {
        int Alayer = floor(AgridID/(xGridCount*yGridCount));
        int Blayer = floor(BgridID/(xGridCount*yGridCount));
        if (Alayer != Blayer) {
            return AgridID > BgridID ? 'U' : 'D';
        } else {
            int layer = Alayer;
            if (layers[layer].isHorizontal()) {
                return AgridID > BgridID ? 'L' : 'R';
            } else {
                return AgridID > BgridID ? 'F' : 'B';
            }
        }
    }

private:
    std::vector<Layer>& layers;
    std::vector<Vertex>& vertices;
    std::vector<Bus>& buses;

private:
    std::vector<std::vector<int>> globalGraph;
    std::vector<GlobalEdge> globalEdges;
    Rectangle& boundary;

private:
    std::vector<std::vector<int>> globalNets;
    std::vector<std::vector<int>> globalNetWidths;

private:
    std::vector<std::vector<std::pair<int, int>>> netOperations;
    std::vector<int> failCount;

private:
    Logger& logger;
    double gridWidth;
    int xGridCount;
    int yGridCount;
    int maxVertexWidth;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
