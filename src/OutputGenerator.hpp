#ifndef VLSI_FINAL_PROJECT_OUTPUT_GENERATOR_HPP_
#define VLSI_FINAL_PROJECT_OUTPUT_GENERATOR_HPP_

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <vector>
#include "Bit.hpp"
#include "Bus.hpp"
#include "Edge.hpp"
#include "FenwickTree.hpp"
#include "GlobalRoutingPath.hpp"
#include "GraphConstructor.hpp"
#include "InputReader.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Net.hpp"
#include "Obstacle.hpp"
#include "Pin.hpp"
#include "Point.hpp"
#include "Rectangle.hpp"
#include "SegmentMap.hpp"
#include "Track.hpp"
#include "Vertex.hpp"

class OutputGenerator {
    class OutputBuffer {
    public:
        void printBuffer(const char* fmt, ...) {
            char buffer[4096];
            va_list args;
            va_start(args, fmt);
            vsprintf(buffer, fmt, args);
            va_end(args);
            if (buffer[strlen(buffer) - 1] == '\n') {
                buffer[strlen(buffer) - 1] = '\0';
            }
            std::string line = buffer;
            lineBuffer.emplace_back(line);
        }

        void dump(FILE* fout) {
            for (const auto& s : lineBuffer) {
                fprintf(fout, "%s\n", s.c_str());
            }
            lineBuffer.clear();
        }

        void clear() {
            lineBuffer.clear();
        }

        int size() {
            return (int) lineBuffer.size();
        }

    private:
        std::vector<std::string> lineBuffer;
    };

public:
    OutputGenerator(std::vector<Vertex>& vertices,
                    std::vector<Bus>& buses,
                    std::vector<Layer>& layers,
                    std::vector<Net>& nets,
                    Logger& logger) :
        vertices(vertices), buses(buses), layers(layers), nets(nets), logger(logger) {}

    void generateOutput(const std::string& outputfile) {
        fout = fopen(outputfile.c_str(), "w");
        if (fout == nullptr) {
            fprintf(stderr, "[ERROR] %s: %s\n", outputfile.c_str(), strerror(errno));
            return;
        }
        for (int i = 0; i < (int) nets.size(); ++i) {
            Net& net = nets[i];
            logger.info("Writing path of bus %s\n", net.netName.c_str());
            fprintf(fout, "BUS %s\n", net.netName.c_str());
            writeSingleNet(net, i);
            fprintf(fout, "ENDBUS\n");
        }
    }

    void writeSingleNet(Net& net, int netId) {
        for (int i = 0; i < (int) net.net.size(); ++i) {
            fprintf(fout, "BIT %d\n", i + 1);
            writeSingleBit(net.detailPath[i], buses[netId].bits[i], buses[netId].widths);
            fprintf(fout, "PATH %d\n", outputBuffer.size());
            outputBuffer.dump(fout);
            fprintf(fout, "ENDPATH\n");
            fprintf(fout, "ENDBIT\n");
        }
    }

    void writeSingleBit(const std::vector<std::vector<int>>& path, const Bit& bit, const std::vector<int>& widths) {
        for (const auto& v : path) {
            writeSingleTwoPinNet(v, bit, widths);
        }
    }

    void writeSingleTwoPinNet(const std::vector<int>& path, const Bit& bit, const std::vector<int>& widths) {
        Rectangle nowLocation = getOverlapRect(path.at(0), bit);
        for (int pathIdx = 1; pathIdx < (int) path.size(); ++pathIdx) {
            const Track& nextTrack = vertices.at(path.at(pathIdx)).track;
            const Track& currentTrack = vertices.at(path.at(pathIdx - 1)).track;
            Rectangle next = nextTrack.rect;
            Rectangle current = currentTrack.rect;
            Rectangle nextLocation = next.overlap(current, false);
            if (nextLocation.isZero()) {
                logger.error("Adjacent vertices don't have overlap area\n");
                return;
            }
            const int width = widths[currentTrack.layer];
            Rectangle area;
            std::pair<Point, Point> coords = getPathCoordinate(nowLocation, nextLocation, currentTrack, width, area);
            int stx = (int) coords.first.x;
            int sty = (int) coords.first.y;
            int edx = (int) coords.second.x;
            int edy = (int) coords.second.y;
            outputBuffer.printBuffer("%s (%d %d) (%d %d)\n", layers[currentTrack.layer].name.c_str(), stx, sty, edx, edy);
            if (currentTrack.layer != nextTrack.layer) {
                int start = std::min(currentTrack.layer, nextTrack.layer);
                int end = std::max(currentTrack.layer, nextTrack.layer);
                Rectangle targetArea = area.overlap(nextLocation, false);
                Point midpoint = targetArea.midPoint();
                for (int layer = start; layer < end; ++layer) {
                    outputBuffer.printBuffer("%s (%d %d)\n", layers[layer].name.c_str(), (int) midpoint.x, (int) midpoint.y);
                }
            }
            nowLocation = nextLocation;
        }
        {
            Rectangle finalLocation = getOverlapRect(path.at(path.size() - 1), bit);
            const Track& currentTrack = vertices.at(path.at(path.size() - 1)).track;
            const int width = widths[currentTrack.layer];
            Rectangle area;
            std::pair<Point, Point> coords = getPathCoordinate(nowLocation, finalLocation, currentTrack, width, area);
            int stx = (int) coords.first.x;
            int sty = (int) coords.first.y;
            int edx = (int) coords.second.x;
            int edy = (int) coords.second.y;
            outputBuffer.printBuffer("%s (%d %d) (%d %d)\n", layers[currentTrack.layer].name.c_str(), stx, sty, edx, edy);
        }
    }

    int getPathCount(const std::vector<std::vector<int>>& path) {
        int num = 0;
        for (const auto& v : path) {
            num += (int) v.size();
        }
        return num;
    }

    Rectangle getOverlapRect(const int pt, const Bit& bit) const {
        const Vertex& v = vertices.at(pt);
        for (const Pin& p : bit.pins) {
            if (v.track.layer == p.layer && v.track.rect.hasOverlap(p.rect, false)) {
                return v.track.rect.overlap(p.rect, false);
            }
        }
        logger.error("The first vertex in path doesn't overlap with any pin of a bit of a bus\n");
        return Rectangle();
    }

    std::pair<Point, Point> getPathCoordinate(const Rectangle& from, const Rectangle& to, const Track& t, const int width, Rectangle& area) const {
        const char direction = layers[t.layer].direction;
        if (direction == 'H') {
            int startx = std::max(std::min(from.ll.x, to.ll.x), t.rect.ll.x);
            int endx = std::min(std::max(from.ur.x, to.ur.x), t.rect.ur.x);
            int ly = (int) t.rect.ll.y;
            while (ly + width <= t.rect.ur.y) {
                Rectangle r(startx, ly, endx, ly + width);
                if (r.hasOverlap(from, false) && r.hasOverlap(to, false)) {
                    area = r;
                    Point pta = Point(startx, ly + width / 2);
                    Point ptb = Point(endx, ly + width / 2);
                    return std::make_pair(pta, ptb);
                }
            }
            logger.error("Cannot find a horizontal path inside the vertex(track)\n");
        } else {
            int starty = std::max(std::min(from.ll.y, to.ll.y), t.rect.ll.y);
            int endy = std::min(std::max(from.ur.y, to.ur.y), t.rect.ur.y);
            int lx = (int) t.rect.ll.x;
            while (lx + width <= t.rect.ur.x) {
                Rectangle r(lx, starty, lx + width, endy);
                if (r.hasOverlap(from, false) && r.hasOverlap(to, false)) {
                    area = r;
                    Point pta = Point(lx + width / 2, starty);
                    Point ptb = Point(lx + width / 2, endy);
                    return std::make_pair(pta, ptb);
                }
            }
            logger.error("Cannot find a vertical path inside the vertex(track)\n");
        }
        return std::make_pair(Point(), Point());
    }

    bool isLineOverlap(double a, double b, double x, double y) const {
        return (a < x && b < x) || (a > y && b > y);
    }

private:
    std::vector<Vertex>& vertices;
    std::vector<Bus>& buses;
    std::vector<Layer>& layers;
    std::vector<Net>& nets;

private:
    FILE* fout;
    OutputBuffer outputBuffer;

private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_OUTPUT_GENERATOR_HPP_
