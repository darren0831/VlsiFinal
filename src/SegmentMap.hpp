#ifndef VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_
#define VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_

#include <algorithm>
#include <utility>
#include <vector>

/**
 * NOTE: This class is currently implemented by a simple method.
 * If there is performance issue, need to re-write this structure
 * using segment tree or something else.
 */
class SegmentMap {
public:
    SegmentMap() :
        lowerBound(0), upperBound(0) {}

    SegmentMap(double lowerBound, double upperBound) :
        lowerBound(lowerBound), upperBound(upperBound) {}

    bool insert(double lower, double upper) {
        bool overlapped = overlapWith(lower, upper);
        if (!overlapped) {
            list.push_back(std::make_pair(lower, upper));
        }
        return !overlapped;
    }

    bool overlapWith(double lower, double upper) {
        if (lower > upperBound || upper < lowerBound) {
            return false;
        }
        lower = std::max(lower, lowerBound);
        upper = std::min(upper, upperBound);
        bool overlapped = false;
        for (const auto& p : list) {
            overlapped = (upper >= p.first && upper <= p.second) ||
                         (lower >= p.first && lower <= p.second);
        }
        return overlapped;
    }

private:
    std::vector<std::pair<double, double>> list;
    double lowerBound;
    double upperBound;
};

#endif // VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_
