#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

class Vertex {
public:
    Vertex() :
        track(Track(-1, -1, -1, -1, -1, -1)), id(-1) {}

    Vertex(const Track& track, int id) :
        track(track), id(id) {}

    Rectangle overlap(const Vertex& that, bool lineOverlap) const {
        return track.rect.overlap(that.track.rect, lineOverlap);
    }

    bool hasOverlap(const Vertex& that, bool lineOverlap) const {
        return track.rect.hasOverlap(that.track.rect, lineOverlap);
    }

    bool operator==(const Vertex& that) const {
        return track == that.track && id == that.id;
    }

    bool operator!=(const Vertex& that) const {
        return !(*this == that);
    }

    std::string toString() {
        return std::to_string(id)+" : "+ track.toString();
    }

public:
    Track track;
    int id;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_HPP_
