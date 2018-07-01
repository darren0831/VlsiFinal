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

    bool query(const Vertex& src, const Vertex& current, const Vertex& tgt) {
        
    }

    void set(const Vertex& src, const Vertex& current, const Vertex& tgt) {
        
    }

    void unset(const Vertex& src, const Vertex& current, const Vertex& tgt) {

    }



private:
    int width;
    std::vector<bool> record;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
