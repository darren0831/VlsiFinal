#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

#include <vector>
#include "Track.hpp"

class Vertex {
public:
    Vertex() :
        track(Track(-1, -1, -1, -1, -1, -1)), id(-1), gridId(std::vector<int>()) {}

    Vertex(const Rectangle& area, int id, int layer, char direction) :
        id(id) {
        gridId = std::vector<int>();
        if (direction == 'H') {
            Point s = Point(area.ll.x, (area.ll.y + area.ur.y) / 2);
            Point t = Point(area.ur.x, (area.ll.y + area.ur.y) / 2);
            track = Track(s.x, s.y, t.x, t.y, area.ur.y - area.ll.y, layer);
        } else {
            Point s = Point((area.ll.x + area.ur.x) / 2, area.ll.y);
            Point t = Point((area.ll.x + area.ur.x) / 2, area.ur.y);
            track = Track(s.x, s.y, t.x, t.y, area.ur.x - area.ll.x, layer);
        }
    }

    Vertex(const Track& track, int id) :
        track(track), id(id), gridId(std::vector<int>()) {}

    Vertex(const Vertex& that) {
        track = that.track;
        id = that.id;
        gridId = that.gridId;
    }

    Vertex(Vertex&& that) noexcept {
        track = std::move(that.track);
        id = that.id;
        gridId = that.gridId;
    }

    Vertex& operator=(const Vertex& that) {
        if (this != &that) {
            track = that.track;
            id = that.id;
            gridId = that.gridId;
        }
        return *this;
    }

    Vertex& operator=(Vertex&& that) noexcept {
        if (this != &that) {
            track = std::move(that.track);
            id = that.id;
            gridId = that.gridId;
        }
        return *this;
    }

    const std::vector<int>& getGridId() const {
        return gridId;
    }

    Rectangle overlap(const Vertex& that, bool lineOverlap) const {
        return track.rect.overlap(that.track.rect, lineOverlap);
    }

    bool hasOverlap(const Vertex& that, bool lineOverlap) const {
        return track.rect.hasOverlap(that.track.rect, lineOverlap);
    }

    bool operator==(const Vertex& that) const {
        return track == that.track && id == that.id && gridId == that.gridId;
    }

    bool operator!=(const Vertex& that) const {
        return !(*this == that);
    }

    std::string toString() const {
        return std::to_string(id) + " : " + track.toString();
    }

public:
    Track track;
    int id;
    std::vector<int> gridId;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_HPP_
