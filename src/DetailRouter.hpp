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
		// for(int i=0;i<(int)nets.size();i++){
		for(int i=0;i<(int)1;i++){
			logger.info("Bus: %d\n",i);
			//for A bus
			int firstBit=-1;
			for(int j=0; j<(int)nets[i].net.size();j++){
				for (int g : vertices[nets[i].net[j][0]].gridId){
					logger.show("%d ",g);
					if(g == globalResult[i][0].gridSequence[0]){
						firstBit=j;
						break;
					}
					if(firstBit!=-1)break;
				}
				logger.show("\n");
			}
			{
				logger.info("  - Bit: %d\n",firstBit);
				std::vector<int> endVertexId;
				std::vector<int>& followedGridId = globalResult[i][0].gridSequence;
				int curGridindex = 0;
				for(int k=1;k<(int)nets[i].net[firstBit].size();k++){
					
					logger.info("    - Net: %d\n",k);
					//for A pin
					candidateVertexId = std::priority_queue<DetailNode>();


					int startVertexId;
					if(k==1)
						startVertexId = nets[i].net[firstBit][0];
					else
						startVertexId = nets[i].net[firstBit][k];

					candidateVertexId.push(DetailNode(0,0,vertices[startVertexId].track.terminal[0].x,
                                           vertices[startVertexId].track.terminal[0].y,vertices[startVertexId].id));	//put source to queue

					if(k==1){
						endVertexId.emplace_back(nets[i].net[firstBit][1]);
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
						bool find = false;
						while(!find && !candidateVertexId.empty()){
							DetailNode curNode = candidateVertexId.top();
							currentVertexId = curNode.nodeId;
							candidateVertexId.pop();
							for(int g : vertices[currentVertexId].getGridId()){
								if(followedGridId[curGridindex] == g) {
									if(curGridindex+1 <= (int)followedGridId.size()-1){
										if(followedGridId[curGridindex+1] == g){
											curGridindex++;
										}
									}
									find = true;
									break;
								}
							}
						}

						for(int end: endVertexId){
							if(currentVertexId == end){
								flag = true;
								break;
							}
						}
						// logger.show("curId: %d\n",currentVertexId);
						
						if(flag)break;
						for(const Edge& e : routingGraph[currentVertexId]){	//put all candidate in queue
							if(prev[e.getTarget()]==-1 && e.getTarget()!=startVertexId){

                                Vertex& nextVertex = vertices[e.getTarget()];
                                if(e.getDirection()=='L'){//left
                                    double nextX = std::max(nextVertex.track.terminal[0].x,nextVertex.track.terminal[1].x);
                                    double nextY = nextVertex.track.terminal[0].y;
                                    double stepDist = curNode.curX - nextX;
                                    double nextDistance = curNode.curDistance + stepDist;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,nextX,nextY,e.getTarget()));
                                }else if(e.getDirection()=='R'){//right
                                    double nextX = std::min(nextVertex.track.terminal[0].x,nextVertex.track.terminal[1].x);
                                    double nextY = nextVertex.track.terminal[0].y;
                                    double stepDist = nextX - curNode.curX;
                                    double nextDistance = curNode.curDistance + stepDist;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,nextX,nextY,e.getTarget()));
                                }else if(e.getDirection()=='B'){///back
                                    double nextX = nextVertex.track.terminal[0].x;
                                    double nextY = std::max(nextVertex.track.terminal[0].y,nextVertex.track.terminal[1].y);
                                    double stepDist = curNode.curY - nextY;
                                    double nextDistance = curNode.curDistance + stepDist;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,nextX,nextY,e.getTarget()));
                                }else if(e.getDirection()=='F'){///forward
                                    double nextX = nextVertex.track.terminal[0].x;
                                    double nextY = std::min(nextVertex.track.terminal[0].y,nextVertex.track.terminal[1].y);
                                    double stepDist = nextY - curNode.curY;
                                    double nextDistance = curNode.curDistance + stepDist;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,nextX,nextY,e.getTarget()));
                                }else if(e.getDirection()=='U'){///up
                                    double nextDistance = curNode.curDistance;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,curNode.curX,curNode.curY,e.getTarget()));
                                }else if(e.getDirection()=='D'){///down
                                    double nextDistance = curNode.curDistance;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,curNode.curX,curNode.curY,e.getTarget()));
                                }else{
                                    double nextDistance = curNode.curDistance;
                                    double nextAssume = 0;
                                    candidateVertexId.push(DetailNode(nextDistance,nextAssume,curNode.curX,curNode.curY,e.getTarget()));
                                }
								//candidateVertexId.push(DetailNode(0,0,0,0,e.getTarget()));
								prev[e.getTarget()] = currentVertexId;
							}
						}
					}
					if(flag){
						logger.info("      Do Detail Back Trace\n");
						while(currentVertexId!=-1){
							// logger.show("%d\n",currentVertexId);
							detailPath.emplace_back(currentVertexId);
							isVertexUsed.insert(currentVertexId);
							currentVertexId = prev[currentVertexId];
						}
					}else{
						logger.error("      No path from source to target\n");
					}

					//TODO: Assign detailPath
				}

			}



			// for(int j=0; j<(int)nets[i].net.size();j++){
			// 	logger.info("  - Bit: %d\n",j);
			// 	//for A bit
			// }

		}

		return;
	}

private:
    struct DetailNode{
        double curDistance;
        double assumeDistance;
        double curX, curY;
        int nodeId;

        DetailNode(double curDist,double assumeDist,double x,double y,int id):
            curDistance(curDist),assumeDistance(assumeDist),curX(x),curY(y),nodeId(id){}

        bool operator<(const DetailNode &that) const{
            double cost = curDistance+assumeDistance;
            double thatCost = that.curDistance + that.assumeDistance;
            if (fabs(cost - thatCost) > 1e-6) {
                return cost < thatCost;
            }else
                return false;
        }
    };

private:
	std::vector<Vertex>& vertices;
	std::vector<std::vector<GlobalRoutingPath>>& globalResult;
	std::vector<Bus>& buses;
	std::vector<Layer>& layers;
	std::vector<std::vector<Edge>>& routingGraph;
	std::vector<Net>& nets;
	Logger& logger;
	std::priority_queue<DetailNode> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<int> detailPath;

};

#endif //VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
