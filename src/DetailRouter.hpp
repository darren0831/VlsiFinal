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
		std::vector<std::vector<int>>& routingGraph,
		std::vector<Edge>& routingEdges,
		std::vector<Net>& nets,
		Logger& logger):
		vertices(vertices), globalResult(globalResult), buses(buses), layers(layers), routingGraph(routingGraph), routingEdges(routingEdges), nets(nets), logger(logger){
		}

    double assumeDistance(const double curX,const double curY, const std::vector<int>& endVertexId){
        double maxX = -1, maxY = -1, minX = -1, minY = -1;
        for(unsigned int i=0;i<endVertexId.size();i++){
            double curX = vertices[endVertexId[i]].track.rect.midPoint().x;
            double curY = vertices[endVertexId[i]].track.rect.midPoint().y;
            if(curX>maxX || maxX == -1 ) maxX = curX;
            if(curY>maxY || maxY == -1 ) maxY = curY;
            if(curX<minX || minX == -1 ) minX = curX;
            if(curY<minY || minY == -1 ) minY = curY;
        }
        maxX  = std::max(maxX, curX);
        maxY  = std::max(maxY, curY);
        minX  = std::min(minX, curX);
        minY  = std::min(minY, curY);
        return (maxX-minX)+(maxY-minY);
    }

	void detailRoute(){
		logger.info("Detail Route\n");
		for(int i=2;i<(int)3;i++){
			logger.info("Bus: %s\n",nets[i].netName.c_str());
			//for A bus
			int firstBit=-1;
			logger.show("globalSequence %s: %d\n",globalResult[i][0].busName.c_str(),globalResult[i][0].gridSequence[0]);
			for(int j=0; j<(int)nets[i].net.size();j++){
				for (int g : vertices[nets[i].net[j][1]].gridId){
					if(g == globalResult[i][0].gridSequence[0]){
						firstBit=j;
						break;
					}
					if(firstBit!=-1)break;
				}
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
					int startVertexId = nets[i].net[firstBit][k];

					candidateVertexId.push(DetailNode(0,0,vertices[startVertexId].track.rect.midPoint().x,
                                           vertices[startVertexId].track.rect.midPoint().y,vertices[startVertexId].id));	//put source to queue

					if(k==1){
						endVertexId.emplace_back(nets[i].net[firstBit][0]);
					}else{
						for(int d: detailPath)
							endVertexId.emplace_back(d);
					}

					bool flag=false;
					std::vector<int> prev = std::vector<int>(vertices.size(),-1);
					int currentVertexId=-1;
					DetailNode curNode;
					while(!candidateVertexId.empty()){
						bool find = false;
						while(!find && !candidateVertexId.empty()){
							curNode = candidateVertexId.top();
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
						for(int eid : routingGraph[currentVertexId]){	//put all candidate in queue
                            int tgtId = routingEdges[eid].getTarget(currentVertexId);
							if(prev[tgtId]==-1 && prev[tgtId]!=startVertexId){
                                Vertex& nextVertex = vertices[tgtId];
                                char edgeDir = routingEdges[eid].getDirection(currentVertexId,tgtId);
                                double nextX = nextVertex.track.rect.midPoint().x;
                                double nextY = nextVertex.track.rect.midPoint().y;
                                double nextDistance, nextAssume;
                                if(edgeDir== 'L' || edgeDir== 'R'){
                                    nextDistance = curNode.curDistance + fabs(curNode.curX - nextX);
                                    nextAssume = assumeDistance(nextX,nextY,endVertexId);
                                }else if(edgeDir== 'F' || edgeDir== 'B'){
                                    nextDistance = curNode.curDistance + fabs(curNode.curY - nextY);
                                    nextAssume = assumeDistance(nextX,nextY,endVertexId);
                                }else if(edgeDir== 'U' || edgeDir== 'D'){
                                    nextDistance = curNode.curDistance;
                                    nextAssume = curNode.assumeDistance;
                                }else{
                                    nextDistance = 0;
                                    nextAssume = 0;
                                }
                                candidateVertexId.push(DetailNode(nextDistance,nextAssume,nextX,nextY,tgtId));
								prev[tgtId] = currentVertexId;
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
        DetailNode(){}

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
    std::vector<std::vector<int>>& routingGraph;
    std::vector<Edge>& routingEdges;
	std::vector<Net>& nets;
	Logger& logger;
	std::priority_queue<DetailNode> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<int> detailPath;

};

#endif //VLSI_FINAL_PROJECT_DETAIL_ROUTER_HPP_
