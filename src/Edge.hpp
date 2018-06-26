#ifndef VLSI_FINAL_PROJECT_EDGE_HPP_
#define VLSI_FINAL_PROJECT_EDGE_HPP_

class Edge {
public:
    Edge(int src, int tgt, char direction, char layerDirection) :
        src(src), tgt(tgt), direction(direction) {
        historicalCost = 0;
        if (layerDirection == 'H') {
            horizontal = true;
            vertical = false;
            via = false;
        } else if (layerDirection == 'V') {
            horizontal = false;
            vertical = true;
            via = false;
        } else if (layerDirection == 'C') {
            horizontal = false;
            vertical = false;
            via = true;
        } else if (layerDirection == 'S') {
            horizontal = false;
            vertical = false;
            via = false;
        } else {
            fprintf(stderr, "[ERROR] Layer Direction '%c' is not defined\n", layerDirection);
        }
    }

    int getTarget(int v) const {
        return (v == tgt) ? src : tgt;
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

    char getDirection() const {
        return direction;
    }

public:
    int src;
    int tgt;
    char direction;
    double historicalCost;

private:
    bool horizontal;
    bool vertical;
    bool via;
};

#endif // VLSI_FINAL_PROJECT_EDGE_HPP_
