#ifndef VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

#include <vector>
#include <stack>

class GraphConstructor {
public:
    GraphConstructor(std::vector<Layer>& layers,
        std::vector<Track>& tracks,
        std::vector<Bus>& buses,
        std::vector<Rectangle>& obstacles,
        Logger& logger) :
        layers(layers), tracks(tracks), buses(buses), obstacles(obstacles) {
        initialize();
    }

    void initialize() {
        logger.info("Graph constructor initialize\n");
        std::vector<std::vector<Track>> layerTracks(layers.size());
        for (const auto& t : tracks) {
            std::stack<Track> stack;
            stack.push(t);
            while (!stack.empty()) {
                Track t = stack.top();
                stack.pop();
                bool hasOverlap = false;
                for (const auto& o : obstacles) {
                    if (t.layer != o.layer) {
                        continue;
                    }
                    Rectangle overlap = t.rect.overlapWith(o.area);
                    if (!overlap.isZero()) {
                        hasOverlap = true;
                        splitTrack(t, overlap, stack);
                    }
                }
                if (!hasOverlap) {
                    layerTracks[t.layer].push_back(t);
                }
            }
        }
        for (int i = 0; i < (int) layerTracks.size(); ++i) {
            for (const auto& track : layerTracks[i]) {

            }
        }
    }

    void splitTrack(const Track& t, const Rectangle& overlap, std::stack<Track>& stack) {

    }

    std::vector<Layer>& layers;
    std::vector<Track>& tracks;
    std::vector<Bus>& buses;
    std::vector<Obstacle>& obstacles;

private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
