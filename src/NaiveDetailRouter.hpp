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
		for(Net& n : nets){
			logger.show("%s:\n",n.netName.c_str());
			int k=0;
			for(int j=0;j<(int)2;j++){
				std::vector<int>& bit = n.net[j];
				k++;
				logger.show("  - Bit %d:\n",k);
				detailPath.clear();
				if(j==0)
					routeFirstBit();
				else
					routeOtherBit(j,bit);
			}
		}
	}

	void routeFirstBit(){
		std::vector<int> endVertexId;
		for(int i=1;i<(int)bit.size();i++){
			logger.show("    - Pin %d:\n",i);
			int startVertexId = bit[i];
			candidateVertexId = std::queue<int>();
			if(i==1)
				endVertexId.emplace_back(bit[0]);
			else{
				for(int d : detailPath)
					endVertexId.emplace_back(d);
			}

			logger.show("      startVertexId: %d\n",startVertexId);
			for(int end : endVertexId){
				logger.show("      endVertexId: %d\n",end);
			}

			candidateVertexId.push(startVertexId);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				currentVertexId = candidateVertexId.front();
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
						candidateVertexId.push(tgtId);
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

		}
	}
	void routeOtherBit(int bitNumber,std::vector<int>& bit){
		std::vector<int> endVertexId;
		for(int i=1;i<(int)bit.size();i++){
			logger.show("    - Pin %d:\n",i);
			int startVertexId = bit[i];
			candidateVertexId = std::queue<int>();
			if(i==1)
				endVertexId.emplace_back(bit[0]);
			else{
				for(int d : detailPath)
					endVertexId.emplace_back(d);
			}

			logger.show("      startVertexId: %d\n",startVertexId);
			for(int end : endVertexId){
				logger.show("      endVertexId: %d\n",end);
			}

			candidateVertexId.push(startVertexId);
			bool flag = false;
			std::vector<int> prev = std::vector<int>(vertices.size(),-1);
			int currentVertexId=-1;
			while(!candidateVertexId.empty()){
				currentVertexId = candidateVertexId.front();
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
						candidateVertexId.push(tgtId);
						prev[tgtId] = currentVertexId;
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

		}
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
	std::queue<int> candidateVertexId;
	std::unordered_set<int> isVertexUsed;
};
#endif