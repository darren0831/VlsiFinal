#ifndef VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_

#include <vector>
#include <unordered_map>
#include "Bus.hpp"
#include "GlobalRoutingPath.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Vertex.hpp"


class DetailRouter{
public:
	DetailRouter(
		std::vector<Vertex>& vertices, 
		std::vector<std::vector<GlobalRoutingPath>>& globalResult,
		std::vector<Bus>& buses,
		std::vector<Layer>& layers,
		std::unordered_map<int, Vertex>& vertexMap,
		Logger& logger):
		vertices(vertices), globalResult(globalResult), buses(buses), layers(layers), vertexMap(vertexMap), logger(logger){}

	void detailRoute(){
		logger.info("Detail Route\n");
		return;
	}

private:
	std::vector<Vertex> vertices;
	std::vector<std::vector<GlobalRoutingPath>> globalResult;
	std::vector<Bus> buses;
	std::vector<Layer> layers;
	std::unordered_map<int, Vertex> vertexMap;
	Logger& logger;
};

#endif