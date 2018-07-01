#ifndef VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_

#include <algorithm>
#include <utility>
#include <vector>
#include "Vertex.hpp"

class VertexUsage {
public:
    VertexUsage() :
        width(0) {}

    VertexUsage(int width) :
        width(width) {
        record = std::vector<bool>(width + 8);
        initialize();
    }

    void initialize() {
        for (int i = 0; i < (int) record.size(); ++i) {
            record[i] = false;
        }
    }

    bool query(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (source == Vertex()) {
            return true;
        }
        auto segment = getSegment(source, current, target);
        checkBound(segment.first, segment.second);
        for (int i = segment.first; i <= segment.second; ++i) {
            if (record[i]) {
                return false;
            }
        }
        return true;
    }

    void set(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (source == Vertex()) {
            return;
        }
        auto segment = getSegment(source, current, target);
        checkBound(segment.first, segment.second);
        for (int i = segment.first; i <= segment.second; ++i) {
            record[i] = true;
        }
    }

    void unset(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (source == Vertex()) {
            return;
        }
        auto segment = getSegment(source, current, target);
        checkBound(segment.first, segment.second);
        for (int i = segment.first; i <= segment.second; ++i) {
            record[i] = false;
        }
    }

    std::pair<int, int> getSegment(const Vertex& source, const Vertex& current, const Vertex& target) const {
        Rectangle r1 = current.track.overlap(source.track);
        Rectangle r2 = current.track.overlap(target.track);
        if (current.track.isHorizontal()) {
            int start = (int) (floor(std::min(r1.ll.x, r2.ll.x)));
            int end = (int) (ceil(std::max(r1.ur.x, r2.ur.x)));
            return std::make_pair(start, end);
        } else {
            int start = (int) (floor(std::min(r1.ll.y, r2.ll.y)));
            int end = (int) (ceil(std::max(r1.ur.y, r2.ur.y)));
            return std::make_pair(start, end);
        }
    }

    void checkBound(int start, int end) {
#ifdef VLSI_FINAL_PROJECT_DEBUG_FLAG
        if (start < 0 || start > width || end < 0 || end > width) {
            fprintf(stderr, "[Fatal Error] Segment [%d, %d] can not be covered by [%d, %d]\n",
                start, end, 0, width);
        }
#endif
    }

private:
    int width;
    std::vector<bool> record;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
