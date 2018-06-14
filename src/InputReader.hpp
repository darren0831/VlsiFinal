#ifndef VLSI_FINAL_PROJECT_INPUT_READER_HPP_
#define VLSI_FINAL_PROJECT_INPUT_READER_HPP_

#include <cerrno>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include "Logger.hpp"
#include "Rectangle.hpp"
#include "Layer.hpp"
#include "Track.hpp"
#include "Bus.hpp"
#include "Bit.hpp"
#include "Pin.hpp"

class InputReader {
public:
    InputReader(std::string filename, Logger& logger) :
        logger(logger) {
        fin = fopen(filename.c_str(), "r");

        if (!fin) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return;
        }

        logger.info("Start reading input file %s\n", filename.c_str());
        readInputFile();
    }

private:
    void readInputFile() {
        readGeneralInfo();
        readLayerInfo();
        readTrackInfo();
        readBusInfo();
        readObstacleInfo();
        printInput();
    }

    void readGeneralInfo() {
        logger.info("Get general info\n");
        fscanf(fin, " RUNTIME %d", &runtime);
        fscanf(fin, " ALPHA %d", &alpha);
        fscanf(fin, " BETA %d", &beta);
        fscanf(fin, " GAMMA %d", &gamma);
        fscanf(fin, " DELTA %d", &delta);
        fscanf(fin, " EPSILON %d", &epsilon);
        fscanf(fin, " DESIGN_BOUNDARY ");
        auto ptA = readPoint();
        auto ptB = readPoint();
        boundary = Rectangle(ptA.first, ptA.second, ptB.first, ptB.second);
    }

    void readLayerInfo() {
        logger.info("Get layer info\n");
        fscanf(fin, " LAYERS %d", &numLayers);

        for (int i = 0; i < numLayers; ++i) {
            char layerName[256];
            char layerType[256];
            int spacing;
            fscanf(fin, "%s %s %d", layerName, layerType, &spacing);
            Layer layer(layerName, spacing, layerType);
            layers.push_back(layer);
            layer_encode[layerName] = i;
        }

        fscanf(fin, " ENDLAYERS");
        logger.info("%d layers read\n", numLayers);
    }

    void readTrackInfo() {
        logger.info("Get track info\n");
        fscanf(fin, " TRACKS %d", &numTracks);

        for (int i = 0; i < numTracks; ++i) {
            char trackLayer[256];
            int width;
            fscanf(fin, "%s", trackLayer);
            auto ptA = readPoint();
            auto ptB = readPoint();
            fscanf(fin, "%d", &width);
            Track track(ptA.first, ptA.second, ptB.first, ptB.second, width, layer_encode[trackLayer]);
            tracks.push_back(track);
        }

        fscanf(fin, " ENDTRACKS");
        logger.info("%d tracks read\n", numTracks);
    }

    void readBusInfo() {
        logger.info("Get bus info\n");
        fscanf(fin, " BUSES %d", &numBuses);

        for (int i = 0; i < numBuses; ++i) {
            char busName[256];
            int numBits;
            int numPins;
            fscanf(fin, " BUS %s", busName);
            fscanf(fin, "%d%d", &numBits, &numPins);
            int numWidth;
            std::vector<int> widths;
            fscanf(fin, " WIDTH %d", &numWidth);

            for (int j = 0; j < numWidth; ++j) {
                int value;
                fscanf(fin, "%d", &value);
                widths.push_back(value);
            }

            fscanf(fin, " ENDWIDTH");
            Bus bus(busName, widths, numBits, numPins);

            for (int j = 0; j < numBits; ++j) {
                char bitId[256];
                fscanf(fin, " BIT %s", bitId);
                Bit bit(bitId);

                for (int k = 0; k < numPins; ++k) {
                    char layerId[256];
                    fscanf(fin, "%s", layerId);
                    auto ptA = readPoint();
                    auto ptB = readPoint();
                    Pin pin(layer_encode[layerId], ptA.first, ptA.second, ptB.first, ptB.second);
                    bit.addPin(pin);
                }

                fscanf(fin, " ENDBIT");
                bus.addBit(bit);
            }

            fscanf(fin, " ENDBUS");
            buses.push_back(bus);
        }

        fscanf(fin, " ENDBUSES");
        logger.info("%d buses read\n", numBuses);
    }

    void readObstacleInfo() {
        logger.info("Get obstacle info\n");
        fscanf(fin, " OBSTACLES %d", &numObstacles);

        for (int i = 0; i < numObstacles; ++i) {
            char layerId[256];
            fscanf(fin, "%s", layerId);
            auto ptA = readPoint();
            auto ptB = readPoint();
            Obstacles obstacle(layerId,ptA.first, ptA.second, ptB.first, ptB.second);
            obstacles.push_back(obstacle);
        }

        fscanf(fin, " ENDOBSTACLES");
        logger.info("%d obstacles read\n", numObstacles);
    }

    std::pair<int, int> readPoint() {
        int x, y;
        fscanf(fin, " (%d %d)", &x, &y);
        return std::make_pair(x, y);
    }

    void printInput() {
        logger.info("Layers:\n");
        for(unsigned i=0;i<layers.size();i++)
        {
            logger.info("%d : %s\n",i,layers[i].print().c_str());
        }
        logger.info("Tracks:\n");
        for(unsigned i=0;i<tracks.size();i++)
        {
            logger.info("%d : %s\n",i,tracks[i].print().c_str());
        }
        logger.info("Buses:\n");
        for(unsigned i=0;i<buses.size();i++)
        {
            logger.info("%d : %s\n",i,buses[i].print().c_str());
        }
        logger.info("Obstacles:\n");
        for (unsigned i = 0; i < obstacles.size(); ++i) {
            logger.info("%d : %s\n", i, obstacles[i].print().c_str());
        }
    }

public:
    int runtime;
    int alpha;
    int beta;
    int gamma;
    int delta;
    int epsilon;
    Rectangle boundary;

public:
    int numLayers;
    std::vector<Layer> layers;
    std::map<std::string, int> layer_encode;

public:
    int numTracks;
    std::vector<Track> tracks;

public:
    int numBuses;
    std::vector<Bus> buses;

public:
    int numObstacles;
    std::vector<Rectangle> obstacles;

private:
    Logger& logger;
    FILE* fin;
};

#endif // VLSI_FINAL_PROJECT_INPUT_READER_HPP_
