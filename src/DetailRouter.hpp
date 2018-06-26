#ifndef VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
#define VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_

#include <vector>
#include <unordered_map>
#include "Bus.hpp"
#include "GlobalRoutingPath.hpp"
#include "Layer.hpp"
#include "Logger.hpp"
#include "Net.hpp"
#include "Vertex.hpp"


class DetailRouter{
public:
	DetailRouter(
		std::vector<Vertex>& vertices, 
		std::vector<std::vector<GlobalRoutingPath>>& globalResult,
		std::vector<Bus>& buses,
		std::vector<Layer>& layers,
		std::unordered_map<int, Vertex>& vertexMap,
		std::vector<std::vector<Vertex>>& routingGraph,
		std::vector<Net>& nets,
		Logger& logger):
		vertices(vertices), globalResult(globalResult), buses(buses), layers(layers), vertexMap(vertexMap), routingGraph(routingGraph), nets(nets), logger(logger){}
	void debug(){
		for(int i=0;i<(int)nets.size();i++){
			logger.show("i:%d\n",i);
			for(int j=0;j<(int)nets[i].net.size();j++){
				logger.show("  j:%d\n",j);
				for(int k=0;k<(int)nets[i].net[j].size();k++){
					logger.show("    k:%d\n",k);
				}
				logger.show("\n");
			}
			logger.show("\n");
		}
	}
	void detailRoute(){
		logger.info("Detail Route\n");
		for(int i=0;i<(int)1;i++){
			logger.info("Bus: %d\n",i);
			//for A bus
			for(int j=0; j<(int)nets[i].net.size();j++){
				logger.info("  - Bit: %d\n",j);
				//for A bit
				std::vector<int> endVertexId;
				for(int k=0;k<(int)nets[i].net[j].size();k++){
					if(k==1) continue;
					logger.info("    - Net: %d\n",k);
					//for A pin
					candidateVertexId = std::priority_queue<int>();
					
					int startVertexId = nets[i].net[j][k];
					candidateVertexId.push(startVertexId);	//put source to queue

					if(k==0){	//put target to endVertexId
						endVertexId.emplace_back(nets[i].net[j][k+1]);
					}else{
						for(int d: detailPath)
							endVertexId.emplace_back(d);
					}
					bool flag=false;
					std::vector<int> prev = std::vector<int>(vertices.size(),-1);
					int currentVertexId=-1;
					while(!candidateVertexId.empty()){
						currentVertexId = candidateVertexId.top();
						// logger.show("curId: %d, %lu\n",currentVertexId,candidateVertexId.size());
						candidateVertexId.pop();
						for(int end: endVertexId){
							if(currentVertexId == end){
								flag=true;
								break;
							}
						}
						if(flag)break;
						for(Vertex& v : routingGraph[currentVertexId]){	//put all candidate in queue
							if(prev[v.id]==-1 && v.id!=startVertexId){
								candidateVertexId.push(v.id);
								prev[v.id] = currentVertexId;
							}
						}
					}
					// logger.info("End ID: %d\n",currentVertexId);
					if(flag){
						// logger.info("Do Detail Back Trace\n");
						while(currentVertexId!=-1){
							detailPath.emplace_back(currentVertexId);
							isVertexUsed.insert(currentVertexId);
							currentVertexId = prev[currentVertexId];
						}
					}else{
						// logger.error("No path from source to target\n");
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
	std::unordered_map<int, Vertex>& vertexMap;
	std::vector<std::vector<Vertex>>& routingGraph;
	std::vector<Net>& nets;
	Logger& logger;
	std::priority_queue<int> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<int> detailPath;
	
};

#endif //VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
