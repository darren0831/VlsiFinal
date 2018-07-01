#ifndef VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_

#include <algorithm>
#include <utility>
#include <vector>
#include "Track.hpp"
#include "Vertex.hpp"

class VertexUsage {
public:
    VertexUsage() :
        width(0) {}

    VertexUsage(int width) :
        width(width) {
        record = std::vector<int>(width + 8);
        initialize();
    }

    void initialize() {
        for (int i = 0; i < (int) record.size(); ++i) {
            record[i] = 0;
        }
    }

    bool query(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (current.track.layer != target.track.layer) {
            return true;
        }
        if (source == Vertex()) {
            for (int i = 0; i < (int) record.size(); ++i) {
                if (record[i]) {
                    return 0;
                }
            }
            return true;
        }
        auto segment = getSegment(source, current, target);
        if (!checkBound(segment.first, segment.second)) {
            printf("%s\n%s\n%s\n", source.toString().c_str(), current.toString().c_str(), target.toString().c_str());
        }
        for (int i = segment.first; i <= segment.second; ++i) {
            if (record[i]) {
                return 0;
            }
        }
        return true;
    }

    void set(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (current.track.layer != target.track.layer) {
            return;
        }
        if (source == Vertex()) {
            for (int i = 0; i < (int) record.size(); ++i) {
                record[i] = 1;
            }
            return;
        }
        auto segment = getSegment(source, current, target);
        checkBound(segment.first, segment.second);
        for (int i = segment.first; i <= segment.second; ++i) {
            record[i] = 1;
        }
    }

    void unset(const Vertex& source, const Vertex& current, const Vertex& target) {
        if (current.track.layer != target.track.layer) {
            return;
        }
        if (source == Vertex()) {
            for (int i = 0; i < (int) record.size(); ++i) {
                record[i] = 0;
            }
            return;
        }
        auto segment = getSegment(source, current, target);
        checkBound(segment.first, segment.second);
        for (int i = segment.first; i <= segment.second; ++i) {
            record[i] = 0;
        }
    }

    std::pair<int, int> getSegment(const Vertex& source, const Vertex& current, const Vertex& target) const {
        Rectangle r1 = current.track.overlap(source.track, false);
        Rectangle r2 = current.track.overlap(target.track, false);
        if (r1.isZero() || r2.isZero()) {
            fprintf(stderr, "[Fatal Error] Source and target have no overlap with current\n");
        }
        if (current.track.isHorizontal()) {
            int start = (int) (floor(std::min(r1.ll.x, r2.ll.x)));
            int end = (int) (ceil(std::max(r1.ur.x, r2.ur.x)));
            start -= current.track.getStartCoordinate();
            end -= current.track.getStartCoordinate();
            return std::make_pair(start, end);
        } else {
            int start = (int) (floor(std::min(r1.ll.y, r2.ll.y)));
            int end = (int) (ceil(std::max(r1.ur.y, r2.ur.y)));
            start -= current.track.getStartCoordinate();
            end -= current.track.getStartCoordinate();
            return std::make_pair(start, end);
        }
    }

    bool checkBound(int start, int end) {
#ifdef VLSI_FINAL_PROJECT_DEBUG_FLAG
        if (start < 0 || start > width || end < 0 || end > width) {
            fprintf(stderr, "[Fatal Error] Segment [%d, %d] can not be covered by [%d, %d]\n",
                start, end, 0, width);
            return false;
        }
#endif
        return true;
    }

private:
    int width;
    std::vector<int> record;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_USAGE_HPP_
