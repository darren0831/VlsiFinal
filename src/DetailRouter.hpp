#ifndef VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_

#include <vector>
#include <unordered_map>
#include <queue>
#include "Bus.hpp"
#include "Edge.hpp"
#include "GlobalRoutingPath.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Net.hpp"
#include "Vertex.hpp"


class DetailRouter {
public:
	DetailRouter(
		std::vector<Vertex>& vertices,
		std::vector<std::vector<GlobalRoutingPath>>& globalResult,
		std::vector<Bus>& buses,
		std::vector<Layer>& layers,
		std::vector<std::vector<Edge>>& routingGraph,
		std::vector<Net>& nets,
		Logger& logger):
		vertices(vertices), globalResult(globalResult), buses(buses), layers(layers), routingGraph(routingGraph), nets(nets), logger(logger){
		}
	void detailRoute(){
		logger.info("Detail Route\n");
		for(int i=0;i<(int)nets.size();i++){
		// for(int i=0;i<(int)1;i++){
			logger.info("Bus: %d\n",i);
			//for A bus
			for(int j=0; j<(int)nets[i].net.size();j++){
			// for(int j=0; j<(int)1;j++){
				logger.info("  - Bit: %d\n",j);
				//for A bit
				std::vector<int> endVertexId;
				for(int k=0;k<(int)nets[i].net[j].size();k++){
					if(k==1) continue;
					logger.info("    - Net: %d\n",k);
					//for A pin
					candidateVertexId = std::queue<int>();

					int startVertexId = nets[i].net[j][k];
					candidateVertexId.push(startVertexId);	//put source to queue

					if(k==0){	//put target to endVertexId
						endVertexId.emplace_back(nets[i].net[j][k+1]);
					}else{
						for(int d: detailPath)
							endVertexId.emplace_back(d);
					}
					// logger.show("startVertexId:%d\n",startVertexId);
					// logger.show("endVertexId:");
					// for(int end: endVertexId){
					// 	logger.show("%d ",end);
					// }
					// logger.show("\n");
					bool flag=false;
					std::vector<int> prev = std::vector<int>(vertices.size(),-1);
					int currentVertexId=-1;
					while(!candidateVertexId.empty()){
						currentVertexId = candidateVertexId.front();
						candidateVertexId.pop();
						// logger.show("curId: %d\n",currentVertexId);
						for(int end: endVertexId){
							if(currentVertexId == end){
								flag = true;
								break;
							}
						}
						if(flag)break;
						for(const Edge& e : routingGraph[currentVertexId]){	//put all candidate in queue
							if(prev[e.getTarget()]==-1 && e.getTarget()!=startVertexId){
								candidateVertexId.push(e.getTarget());
								prev[e.getTarget()] = currentVertexId;
							}
						}
					}
					if(flag){
						logger.info("      Do Detail Back Trace\n");
						while(currentVertexId!=-1){
							// logger.show("%d\n",currentVertexId);
							detailPath.emplace_back(currentVertexId);
							// isVertexUsed.insert(currentVertexId);
							currentVertexId = prev[currentVertexId];
						}
					}else{
						logger.error("      No path from source to target\n");
					}

					//TODO: Assign detailPath
				}
			}

		}

		return;
	}

private:
	std::vector<Vertex>& vertices;
	std::vector<std::vector<GlobalRoutingPath>>& globalResult;
	std::vector<Bus>& buses;
	std::vector<Layer>& layers;
	std::vector<std::vector<Edge>>& routingGraph;
	std::vector<Net>& nets;
	Logger& logger;
	std::queue<int> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<int> detailPath;

};

#endif //VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
