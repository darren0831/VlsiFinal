#ifndef VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_

#include <algorithm>
#include <vector>

class VertexUsage {
public:
    VertexUsage() :
        width(0) {}

    VertexUsage(int width) :
        width(width) {
        record = std::vector<bool>(width);
    }

    void initialize() {
        for (int i = 0; i < (int) record.size(); ++i) {
            record[i] = false;
        }
    }

    bool query(int from, int to) {
        if (from == -1 && to == -1) {
            return true;
        }
        int a = std::min(from, to);
        int b = std::max(from, to);
        for (int i = a; i <= b; ++i) {
            if (record[i]) {
                return false;
            }
        }
        return true;
    }

    void set(int from, int to) {
        if (from == -1 && to == -1) {
            return;
        }
        int a = std::min(from, to);
        int b = std::max(from, to);
        for (int i = a; i <= b; ++i) {
            record[i] = true;
        }
    }

    void unset(int from, int to) {
        int a = std::min(from, to);
        int b = std::max(from, to);
        for (int i = a; i <= b; ++i) {
            record[i] = false;
        }
    }

private:
    int width;
    std::vector<bool> record;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
