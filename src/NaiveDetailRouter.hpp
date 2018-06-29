#ifndef VLSI_FINAL_PROJECT_NAIVE_DETAIL_ROUTER_PATH_HPP_
#define VLSI_FINAL_PROJECT_NAIVE_DETAIL_ROUTER_PATH_HPP_
class NaiveDetailRouter {
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
				std::vector<int>& bit = n.net[j];
				k++;
				logger.show("  - Bit %d:\n",k);
				curNet.clear();
				if(j==0)
					flag = routeFirstBit(bit,n);
				else{
					if(flag)
						routeOtherBit(j,bit,n);
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
			candidateVertexId = std::queue<DetailNode>();
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
			DetailNode start(startVertexId,-1);
			candidateVertexId.push(start);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				DetailNode currentNode = candidateVertexId.front();
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
						candidateVertexId.push(DetailNode(tgtId,-1));
						prev[tgtId] = currentVertexId;
					}
				}
			}
			if(flag){
				logger.show("      Do Detail Back Trace\n");
				while(currentVertexId!=-1){
					// logger.show("%d\n",currentVertexId);
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
			candidateVertexId = std::queue<DetailNode>();
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
			DetailNode start(startVertexId,0);
			candidateVertexId.push(start);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				DetailNode currentNode = candidateVertexId.front();
				currentVertexId = currentNode.vertexId;
				curFollowId = currentNode.followId;
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
						if((direction[i-1][curFollowId]==routingEdges[eid].getDirection(currentVertexId,tgtId)||routingEdges[eid].getDirection(currentVertexId,tgtId)==' ') && curFollowId < (int)direction[i-1].size()){
							candidateVertexId.push(DetailNode(tgtId,curFollowId+1));
							prev[tgtId] = currentVertexId;
						}
					}
				}
			}
			
			if(flag){
				logger.show("      Do Detail Back Trace\n");
				while(currentVertexId!=-1){
					// logger.show("%d\n",currentVertexId);
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
		DetailNode(int vertexId,int followId):vertexId(vertexId),followId(followId){}
		int vertexId;
		int followId;
	};

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
	std::queue<DetailNode> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
	std::vector<std::vector<char>> direction;
	std::unordered_set<int> curNet; 
};
#endif