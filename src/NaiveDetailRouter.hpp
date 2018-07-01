#ifndef VLSI_FINAL_PROJECT_NAIVE_DETAIL_ROUTER_PATH_HPP_
#define VLSI_FINAL_PROJECT_NAIVE_DETAIL_ROUTER_PATH_HPP_
class NaiveDetailRouter {
private:
	int VIA_COST=0;
public:
	NaiveDetailRouter(std::vector<Vertex>& vertices,
		std::vector<Bus>& buses,
		std::vector<Layer>& layers,
		std::vector<std::vector<int>>& routingGraph,
		std::vector<Edge>& routingEdges,
		std::vector<Net>& nets,
		Logger& logger):
		vertices(vertices), buses(buses), layers(layers), routingGraph(routingGraph), routingEdges(routingEdges), nets(nets), logger(logger){
		}
	void detailRoute(){
		logger.info("Detail Route\n");
		int c=0;
		for(Net& n : nets){
			c++;
			// if(c<=3)continue;
			logger.show("%s:\n",n.netName.c_str());
			n.detailPath = std::vector<std::vector<std::vector<int>>>(n.net.size());
			int k=0;
			bool flag = false;
			for(int j=0;j<(int)n.net.size();j++){
				k++;
				logger.show("  - Bit %d:\n",k);
				curNet.clear();
				if(j==0){
					flag = routeFirstBit(n.net[j],n);
				}
				else{
					if(flag)
						routeOtherBit(j,n.net[j],n);

				}
			}
		}
	}

	bool routeFirstBit(std::vector<int>& bit, Net& net){
		direction = std::vector<std::vector<char>>((int)bit.size()-1);

		for(int i=1;i<(int)bit.size();i++){
			std::vector<int> endVertexId;
			logger.show("    - Pin %d:\n",i);
			int startVertexId = bit[i];
			candidateVertexId = std::priority_queue<DetailNode>();
			if(i==1)
				endVertexId.emplace_back(bit[0]);
			else{
				for(int d : curNet){
					endVertexId.emplace_back(d);
				}
			}

			// logger.show("      startVertexId: %d\n",startVertexId);
			// for(int end : endVertexId){
			// 	logger.show("      endVertexId: %d\n",end);
			// }
			detailPath.clear();
			double x = vertices[startVertexId].track.rect.midPoint().x;
			double y = vertices[startVertexId].track.rect.midPoint().y;
			DetailNode start(x,y,0,0,startVertexId,-1);
			candidateVertexId.push(start);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			std::vector<int> preveid = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				DetailNode currentNode = candidateVertexId.top();
				currentVertexId = currentNode.vertexId;
				candidateVertexId.pop();
				for(int end: endVertexId){
					if(currentVertexId == end){
						flag = true;
						break;
					}
				}
				if(flag)break;
				for(int eid : routingGraph[currentVertexId]){
					int tgtId = routingEdges[eid].getTarget(currentVertexId);
					if(prev[tgtId] == -1 && prev[currentVertexId]!=tgtId && prev[tgtId]!=startVertexId && isVertexUsed.find(tgtId)==isVertexUsed.end()){
						if(vertices[tgtId].track.width>=net.widths[vertices[tgtId].track.layer]){
							double nextX = vertices[tgtId].track.rect.midPoint().x;
	                        double nextY = vertices[tgtId].track.rect.midPoint().y;
	                        std::pair<double,double> distanceCost = calDistanceCost( eid, tgtId, currentVertexId, currentNode, endVertexId);
							candidateVertexId.push(DetailNode(nextX, nextY, distanceCost.first, distanceCost.second, tgtId, -1));
							prev[tgtId] = currentVertexId;
							preveid[tgtId] = eid;
						}	
					}
				}
			}
			if(flag){
				logger.show("      Do Detail Back Trace\n");
				while(currentVertexId!=-1){
					detailPath.emplace_back(currentVertexId);
					curNet.insert(currentVertexId);
					isVertexUsed.insert(currentVertexId);
					currentVertexId = prev[currentVertexId];
				}
				std::reverse(detailPath.begin(),detailPath.end());
				for(int j=0;j<(int)detailPath.size()-1;j++){
					for(int e: routingGraph[detailPath[j]]){
						if(routingEdges[e].getTarget(detailPath[j])==detailPath[j+1]){
							if(routingEdges[e].getDirection(detailPath[j],detailPath[j+1])!=' '){
								direction[i-1].emplace_back(routingEdges[e].getDirection(detailPath[j],detailPath[j+1]));
								// logger.show("%d, %c\n",detailPath[j],routingEdges[e].getDirection(detailPath[j],detailPath[j+1]));
								break;
							}
						}
					}
					// logger.show("\n");	
				}
				
				net.detailPath[0].emplace_back(detailPath);
			}else{
				logger.show("      No path from source to target\n");
				return false;
			}
		}
		return true;
	}
	void routeOtherBit(int bitNumber,std::vector<int>& bit, Net& net){
		for(int i=1;i<(int)bit.size();i++){
			std::vector<int> endVertexId;
			logger.show("    - Pin %d:\n",i);
			int startVertexId = bit[i];
			int curFollowId = 0;
			candidateVertexId = std::priority_queue<DetailNode>();

			if(i==1)
				endVertexId.emplace_back(bit[0]);
			else{
				for(int d : curNet){
					endVertexId.emplace_back(d);
				}
			}

			// logger.show("      startVertexId: %d\n",startVertexId);
			// for(int end : endVertexId){
			// 	logger.show("      endVertexId: %d\n",end);
			// }
			detailPath.clear();
			double x = vertices[startVertexId].track.rect.midPoint().x;
			double y = vertices[startVertexId].track.rect.midPoint().y;
			DetailNode start(x,y,0,0,startVertexId,0);
			candidateVertexId.push(start);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			std::vector<int> preveid = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				DetailNode currentNode = candidateVertexId.top();
				currentVertexId = currentNode.vertexId;
				curFollowId = currentNode.followId;
				// logger.show("%d\n",currentVertexId);
				candidateVertexId.pop();
				for(int end: endVertexId){
					if(currentVertexId == end){
						flag = true;
						break;
					}
				}
				if(flag)break;
				for(int eid : routingGraph[currentVertexId]){
					int tgtId = routingEdges[eid].getTarget(currentVertexId);
					if(prev[tgtId] == -1 && prev[currentVertexId]!=tgtId && prev[tgtId]!=startVertexId && isVertexUsed.find(tgtId)==isVertexUsed.end()){
						if(direction[i-1].size()==0){
							if(vertices[tgtId].track.width>=net.widths[vertices[tgtId].track.layer]){
								double nextX = vertices[tgtId].track.rect.midPoint().x;
		                        double nextY = vertices[tgtId].track.rect.midPoint().y;
		                        std::pair<double,double> distanceCost = calDistanceCost(eid, tgtId, currentVertexId, currentNode, endVertexId);
								candidateVertexId.push(DetailNode(nextX, nextY, distanceCost.first, distanceCost.second, tgtId, curFollowId));
								prev[tgtId] = currentVertexId;
								preveid[tgtId] = eid;
							}
						}else{
							if(direction[i-1][curFollowId]==routingEdges[eid].getDirection(currentVertexId,tgtId)||routingEdges[eid].getDirection(currentVertexId,tgtId)==' '){
								if(vertices[tgtId].track.width>=net.widths[vertices[tgtId].track.layer]){
									double nextX = vertices[tgtId].track.rect.midPoint().x;
			                        double nextY = vertices[tgtId].track.rect.midPoint().y;
			                        std::pair<double,double> distanceCost = calDistanceCost( eid, tgtId, currentVertexId, currentNode, endVertexId);
			                        if(routingEdges[eid].getDirection(currentVertexId,tgtId)==' ')
			                    		candidateVertexId.push(DetailNode(nextX, nextY, distanceCost.first, distanceCost.second, tgtId, curFollowId));
									else
										candidateVertexId.push(DetailNode(nextX, nextY, distanceCost.first, distanceCost.second, tgtId, curFollowId+1));
									prev[tgtId] = currentVertexId;
									preveid[tgtId] = eid;
								}
							}
						}
					}
				}
			}
			
			if(flag){
				logger.show("      Do Detail Back Trace\n");
				while(currentVertexId!=-1){
					detailPath.emplace_back(currentVertexId);
					curNet.insert(currentVertexId);
					isVertexUsed.insert(currentVertexId);
					currentVertexId = prev[currentVertexId];
				}
				std::reverse(detailPath.begin(),detailPath.end());
				net.detailPath[bitNumber].emplace_back(detailPath);
			}else{
				logger.show("      No path from source to target\n");
			}

		}
	}
private:
	class DetailNode{
	public:
		int curX, curY;
		double curDistance;
        double assumeDistance;
        int vertexId;
		int followId;
	public:
		DetailNode(int curX, int curY, double curDistance, double assumeDistance, int vertexId,int followId):
		curX(curX), curY(curY), curDistance(curDistance), assumeDistance(assumeDistance), vertexId(vertexId),followId(followId){}
        bool operator<(const DetailNode &that) const{
            double cost = curDistance+assumeDistance;
            double thatCost = that.curDistance + that.assumeDistance;
            if (fabs(cost - thatCost) > 1e-6) {
                return cost < thatCost;
            }else
                return false;
        }
    
	};
public:
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
    std::pair<double, double> calDistanceCost(const int& eid, const int& tgtId,const int& currentVertexId, const DetailNode& curNode, const std::vector<int>& endVertexId){
        char edgeDir = routingEdges[eid].getDirection(currentVertexId,tgtId);
        Vertex& nextVertex = vertices[tgtId];
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
            nextAssume = curNode.assumeDistance + VIA_COST;
        }else{
            nextDistance = 0;
            nextAssume = 0;
        }
        return std::make_pair(nextDistance,nextAssume);
    }
public:
	std::vector<Vertex>& vertices;
	std::vector<Bus>& buses;
	std::vector<Layer>& layers;
	std::vector<std::vector<int>>& routingGraph;
	std::vector<Edge>& routingEdges;
	std::vector<Net>& nets;
public:
	Logger& logger;
	std::vector<int> detailPath;
	std::priority_queue<DetailNode> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<std::vector<char>> direction;
	std::unordered_set<int> curNet; 
};
#endif