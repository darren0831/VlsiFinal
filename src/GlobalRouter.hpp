#ifndef VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cmath>
#include "Vertex.hpp"
#include "Layer.hpp"
#include "FenwickTree.hpp"

class GlobalRouter {
private:
    class GlobalEdge {
    public:
        GlobalEdge(int id, int src, int tgt, int maxVertexWidth) : id(id), src(src), tgt(tgt),
        maxWidth(maxVertexWidth) {
            cost = 0;
            historical_cost = 0;
            ft = FenwickTree(maxWidth);
        }

        int getTarget(int source) const {
            return (source == src) ? tgt : src;
        }

        double getCost() { return cost; }

        void setCost(double cost) { this->cost = cost; }

        double getHistoricalCost() { return historical_cost; }

        void setHistoricalCost(double historical_cost) { this->historical_cost = historical_cost; }

        int edgeRequest(int count, int width) {
            return 0;
        }

        void edgeRecover(int operId) {

        }

        int edgeCount(int querywidth) {
            if (layer == -1) { return std::numeric_limits<int>::max(); }
            return ft.query(querywidth);
        }

        void addVertexToEdge(Vertex v) {
            vertices[v.id] = (int) ceil(v.track.width);
            ft.insert((int) v.track.width, 1);
        }

    public:
        int src;
        int tgt;
        int id;
        int layer;
    private:
        double cost;
        double historical_cost;
        std::unordered_map<int, int> vertices;
        FenwickTree ft;
        int maxWidth;


    };

public:
    GlobalRouter(std::vector<Layer>& layers,
       std::vector<Vertex>& vertices,
       std::unordered_map<int, Vertex>& vertexMap,
       std::vector<std::vector<Vertex>>& routingGraph,
       std::vector<Net>& nets,
       std::vector<Bus>& buses,
       Rectangle& boundary,
       Logger& logger) :
    layers(layers), vertices(vertices), vertexMap(vertexMap), routingGraph(routingGraph), nets(nets),
    buses(buses), boundary(boundary), logger(logger) {}

    void globalRoute() {
        logger.info("Initialize\n");
        initialize();
        logger.info("Prepare nets\n");
        prepareNets();
        logger.info("Do global route\n");
        doRouting();
    }

private:
    struct VisitNode {
        int nodeId;
        int from;
        double cost;

        VisitNode(int nodeId, int from, double cost) :
            nodeId(nodeId), from(from), cost(cost) {}

        bool operator<(const VisitNode& that) const {
            return cost < that.cost || (fabs(cost - that.cost) < 1e-6 && nodeId < that.nodeId);
        }
    };

private:
    void initialize() {
        logger.info("Calculate Max Vertex Width\n");
        calMaxVertexWidth();
        logger.info("Construct global edges\n");
        constructGlobalEdge();
        logger.info("Bind vertex to edges\n");
        bindVertexToEdge();
    }

    void printGlobalEdge(){
        Logger logs;
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
                width.emplace_back(bus.widths[i] * bus.numBits);
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

    void doRouting() {
        for (int i = 0; i < (int) globalNets.size(); ++i) {
            logger.info("Routing bus %s\n", buses[i].name.c_str());
            routeSingleNet(globalNets[i], globalNetWidths[i], buses[i].numBits);
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
        gridWidth = calGridWidth() * 100;
        xGridCount = (int) ceil(boundary.ur.x/gridWidth);
        yGridCount = (int) ceil(boundary.ur.y/gridWidth);
        int edgeId=0;
        globalGraph = std::vector<std::vector<int>>(xGridCount*yGridCount*(int)layers.size());
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
                        if(j<yGridCount-1){
                            GlobalEdge lr(edgeId,(i*xGridCount+j) + k*xGridCount*yGridCount,(i*xGridCount+(j+1)) + k*xGridCount*yGridCount,maxVertexWidth);
                            lr.layer = k;
                            globalEdges.emplace_back(lr);
                            globalGraph[(i * xGridCount + j) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            globalGraph[(i * xGridCount + (j+1)) + k*xGridCount*yGridCount].emplace_back(edgeId);
                            ++edgeId;
                        }
                    }else {
                        if(i<xGridCount-1){
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
            // logger.show("%s:%d - %d\n",v.toString().c_str(),fromGridId,toGridId);
            char vertexDir = getDirection(fromGridId, layer, toGridId, layer);
            if (layers[layer].isHorizontal()) {
                for (int i = fromGridId; i < toGridId; ++i) {
                    for (int j : globalGraph[i]) {
                        if (getDirection(globalEdges[j].src, layer, globalEdges[j].tgt, layer)) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                    for (int j : globalGraph[i + 1]) {
                        if (getDirection(globalEdges[j].src, layer, globalEdges[j].tgt, layer)) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                }
            } else {
                for (int i = fromGridId; i < toGridId - xGridCount; i += xGridCount) {
                    for (int j : globalGraph[i]) {
                        if (getDirection(globalEdges[j].src, layer, globalEdges[j].tgt, layer) == vertexDir) {
                            globalEdges[j].addVertexToEdge(v);
                        }
                    }
                    for (int j : globalGraph[i + xGridCount]) {
                        if (getDirection(globalEdges[j].src, layer, globalEdges[j].tgt, layer) == vertexDir) {
                            globalEdges[j].addVertexToEdge(v);

                        }
                    }
                }
            }
        }
    }

    bool routeSingleNet(const std::vector<int>& net, const std::vector<int>& widths, const int numBits) {
        std::vector<int> tgt;
        tgt.emplace_back(net.at(0));
        for (int i = 1; i < (int) net.size(); ++i) {
            int src = net[i];
            if (std::count(tgt.begin(), tgt.end(), src) > 0) {
                continue;
            }
            printf("from %d to ", src);
            for (int v : tgt) {
                printf(" %d", v);
            }
            puts("");
            auto result = routeSinglePath(src, tgt, widths, numBits);
            if (!result.empty()) {
                for (int v : result) {
                    tgt.emplace_back(v);
                }
            } else {
                logger.error("Fuck! Do rip-up reroute\n");
                return false;
            }
        }
        return true;
    }


    std::vector<int> routeSinglePath(const int src, const std::vector<int>& target, const std::vector<int>& width, const int bitCount) {
        std::priority_queue<VisitNode> pQ;
        bool visited[globalGraph.size()];
        bool targetVertex[globalGraph.size()];
        std::vector<double> gridCost(globalGraph.size());
        std::vector<int> predecessor(globalGraph.size());
        for (int i = 0; i < (int) globalGraph.size(); ++i) {
            visited[i] = false;
            targetVertex[i] = false;
            gridCost[i] = 1e6;
        }
        for (int v : target) {
            targetVertex[v] = true;
        }
        bool found = false;
        int finalVertex = -1;
        gridCost[src] = 0;
        pQ.push(VisitNode(src, -1, 0));
        while (!pQ.empty()) {
            VisitNode top = pQ.top();
            pQ.pop();
            int node = top.nodeId;
            if (targetVertex[node]) {
                found = true;
                finalVertex = node;
                predecessor[node] = top.from;
                break;
            }
            if (visited[node]) {
                continue;
            }
            visited[node] = true;
            predecessor[node] = top.from;
            for (int next : globalGraph[node]) {
                int out = (node == globalEdges[next].tgt) ? globalEdges[next].src : globalEdges[next].tgt;
                if (visited[out]) {
                    continue;
                }
                if (gridCost[node] + globalEdges[next].getCost() < gridCost[out]) {
                    gridCost[out] = gridCost[node] + globalEdges[next].getCost();
                    pQ.push(VisitNode(out, node, gridCost[out]));
                }
            }
        }
        if (!found || finalVertex == -1) {
            return std::vector<int>();
        }
        std::vector<int> path;
        printf("%d ", finalVertex);
        while (finalVertex != -1) {
            path.emplace_back(finalVertex);
            finalVertex = predecessor[finalVertex];
            printf("%d ", finalVertex);
        }
        printf("\n");
        std::reverse(path.begin(), path.end());
        return path;
    }

    int coordToGridId(Point p, int layer) {
        int x = (int) floor(p.x / gridWidth);
        int y = (int) floor(p.y / gridWidth);
        return (y * xGridCount + x) + layer * xGridCount * yGridCount;
    }

    char getDirection(int AgridID, int Alayer, int BgridID, int Blayer) {
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
    std::unordered_map<int, Vertex>& vertexMap;
    std::vector<std::vector<Vertex>>& routingGraph;
    std::vector<Net>& nets;
    std::vector<Bus>& buses;

private:
    std::vector<std::vector<int>> globalGraph;
    std::vector<GlobalEdge> globalEdges;
    Rectangle& boundary;

private:
    std::vector<std::vector<int>> globalNets;
    std::vector<std::vector<int>> globalNetWidths;

private:
    Logger& logger;
    double gridWidth;
    int xGridCount, yGridCount;
    int maxVertexWidth;
};

#endif //VLSI_FINAL_PROJECT_GLOBAL_ROUTER_HPP_
