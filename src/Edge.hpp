#ifndef VLSI_FINAL_PROJECT_EDGE_HPP_
#define VLSI_FINAL_PROJECT_EDGE_HPP_

class Edge {
public:
    Edge(int src, int tgt, char srcToTgtDir, char tgtToSrcDir, char layerDirection) :
        src(src), tgt(tgt), srcToTgtDir(srcToTgtDir), tgtToSrcDir(tgtToSrcDir) {
        historicalCost = 0;
        if (layerDirection == 'H') {        // horizontal
            horizontal = true;
            vertical = false;
            via = false;
        } else if (layerDirection == 'V') { // vertical
            horizontal = false;
            vertical = true;
            via = false;
        } else if (layerDirection == 'C') { // via
            horizontal = false;
            vertical = false;
            via = true;
        } else if (layerDirection == 'S') { // special (only from 'source', no topology)
            horizontal = false;
            vertical = false;
            via = false;
        } else {
            fprintf(stderr, "[ERROR] Layer Direction '%c' is not defined\n", layerDirection);
        }
    }

    int getTarget(int source) const {
        return (source == tgt) ? src : tgt;
    }

    double getHistoricalCost() const {
        return historicalCost;
    }

    void setHistoricalCost(double cost) {
        historicalCost = cost;
    }

    char getEdgeType() const {
        if (horizontal) {
            return 'H';
        } else if (vertical) {
            return 'V';
        } else if (via) {
            return 'C';
        } else {
            return 'S';
        }
    }

    bool isHorizontal() const {
        return horizontal;
    }

    bool isVertical() const {
        return vertical;
    }

    bool isVia() const {
        return via;
    }

    bool withoutTopology() const {
        return !horizontal && !vertical && !via;
    }

    char getDirection(int source, int target) const {
        if (source == src && target == tgt) {
            return srcToTgtDir;
        } else {
            return tgtToSrcDir;
        }
    }

public:
    int src;
    int tgt;
    char srcToTgtDir;
    char tgtToSrcDir;
    double historicalCost;

private:
    bool horizontal;
    bool vertical;
    bool via;
};

#endif // VLSI_FINAL_PROJECT_EDGE_HPP_
