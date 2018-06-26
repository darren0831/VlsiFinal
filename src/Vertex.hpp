#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

#include <vector>
#include "Track.hpp"

class Vertex {
public:
    Vertex() :
        track(Track(-1, -1, -1, -1, -1, -1)), id(-1), gridId(std::vector<int>()) {}

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

    void setGridId(const std::vector<int>& id) {
        gridId = id;
    }

    std::vector<int> getGridId() const {
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
