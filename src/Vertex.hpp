#ifndef VLSI_FINAL_PROJECT_VERTEX_HPP_
#define VLSI_FINAL_PROJECT_VERTEX_HPP_

class Vertex {
public:
    Vertex() :
        track(Track(-1, -1, -1, -1, -1, -1)), id(-1) {}

    Vertex(const Track& track, int id) :
        track(track), id(id) {}

    Rectangle overlapWith(const Vertex& that) const {
        return track.rect.overlapWith(that.track.rect);
    }

    bool hasOverlapWith(const Vertex& that) const {
        return track.rect.hasOverlapWith(that.track.rect);
    }

    bool operator==(const Vertex& that) const {
        return track == that.track && id == that.id;
    }

    bool operator!=(const Vertex& that) const {
        return !(*this == that);
    }

public:
    Track track;
    int id;
};

#endif // VLSI_FINAL_PROJECT_VERTEX_HPP_
