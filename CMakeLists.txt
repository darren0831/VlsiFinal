cmake_minimum_required(VERSION 3.8)
project(VlsiFinalProject)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/Bit.hpp
        src/Bus.hpp
        src/Edge.hpp
        src/FenwickTree.hpp
        src/GlobalRouter.hpp
        src/GlobalRoutingPath.hpp
        src/GraphConstructor.hpp
        src/InputReader.hpp
        src/Layer.hpp
        src/Logger.hpp
        src/main.cpp
        src/Net.hpp
        src/Obstacle.hpp
        src/Pin.hpp
        src/Point.hpp
        src/Preprocess.hpp
        src/Rectangle.hpp
        src/SegmentMap.hpp
        src/Track.hpp
        src/Vertex.hpp)

add_executable(route ${SOURCE_FILES})
