#ifndef VLSI_FINAL_PROJECT_INPUT_READER_HPP_
#define VLSI_FINAL_PROJECT_INPUT_READER_HPP_

#include <cerrno>
#include <string>
#include <utility>
#include <vector>
#include "Logger.hpp"
#include "Rectangle.hpp"

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
            // TODO record this infomation
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
            Point a(ptA.first, ptA.second);
            Point b(ptB.first, ptB.second);
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
            for (int j = 0; j < numBits; ++j) {
                int bitId;
                fscanf(fin, " BIT %d", &bitId);
                for (int k = 0; k < numPins; ++k) {
                    char layerId[256];
                    fscanf(fin, "%s", layerId);
                    auto ptA = readPoint();
                    auto ptB = readPoint();
                }
                fscanf(fin, " ENDBIT");
            }
            fscanf(fin, " ENDBUS");
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
        }
        fscanf(fin, " ENDOBSTACLES");
        logger.info("%d obstacles read\n", numObstacles);
    }

    std::pair<int, int> readPoint() {
        int x, y;
        fscanf(fin, " (%d %d)", &x, &y);
        return std::make_pair(x, y);
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

public:
    int numTracks;

public:
    int numBuses;

public:
    int numObstacles;

private:
    Logger& logger;
    FILE* fin;
};

#endif // VLSI_FINAL_PROJECT_INPUT_READER_HPP_
