#ifndef VLSI_FIANL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FIANL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

class GraphConstructor {
public:
    GraphConstructor(std::vector<Layer>& layers,
        std::vector<Track>& tracks,
        std::vector<Bus>& buses,
        std::vector<Rectangle>& obstacles) :
        layers(layers), tracks(tracks), buses(buses), obstacles(obstacles) {
        initialize();
    }

    std::vector<Layer>& layers;
    std::vector<Track>& tracks;
    std::vector<Bus>& buses;
    std::vector<Rectangle>& obstacles;
};

#endif // VLSI_FIANL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
