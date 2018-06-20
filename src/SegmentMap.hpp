#ifndef VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_
#define VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_

#include <algorithm>
#include <utility>
#include <vector>
#include "Rectangle.hpp"

/**
 * NOTE: This class is currently implemented by a simple method.
 * If there is performance issue, need to re-write this structure
 * using segment tree or something else.
 */
class SegmentMap {
public:
    SegmentMap() = default;

    SegmentMap(const Rectangle& rect) :
        bound(rect) {}

    bool insert(const Rectangle& rect) {
        bool overlapped = overlapWith(rect);
        if (!overlapped) {
            insertIntoList(rect);
        }
        return !overlapped;
    }

    bool overlapWith(const Rectangle& rect) {
        if (!rect.hasOverlap(bound)) {
            return false;
        }
        for (const auto& p : list) {
            if (rect.isCoveredBy(p)) {
                return true;
            }
        }
        return false;
    }

private:
    void insertIntoList(const Rectangle& rect) {
        list.emplace_back(rect);
    }

private:
    std::vector<Rectangle> list;
    Rectangle bound;
};

#endif // VLSI_FINAL_PROJECT_SEGMENT_MAP_HPP_
