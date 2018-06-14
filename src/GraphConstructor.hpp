#ifndef VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
#define VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_

#include <vector>
#include <stack>

class GraphConstructor {
public:
    GraphConstructor(std::vector<Layer>& layers,
        std::vector<Track>& tracks,
        std::vector<Bus>& buses,
        std::vector<Rectangle>& obstacles,
        Logger& logger) :
    layers(layers), tracks(tracks), buses(buses), obstacles(obstacles) {
        initialize();
    }

    void initialize() {
        logger.info("Graph constructor initialize\n");
        std::vector<Track> all_tracks;
        for (const auto& t : tracks) {
            std::stack<Track> stack;
            stack.push(t);
            while (!stack.empty()) {
                Track t = stack.top();
                stack.pop();
                bool hasOverlap = false;
                for (const auto& o : obstacles) {
                    if (t.layer != o.layer) {
                        continue;
                    }
                    Rectangle overlap = t.rect.overlapWith(o.area);
                    if (!overlap.isZero()) {
                        hasOverlap = true;
                        splitTrack(t, overlap, stack);
                    }
                }
                if (!hasOverlap) {
                    all_tracks.push_back(t);
                }
            }
        }
        int vertextId=0;
        for(unsigned i=0;i<all_tracks.size();i++)
        {
            for(unsigned j=i+1;j<all_tracks.size();j++)
            {
                Rectangle overlap = all_tracks[i].rect.overlapWith(all_tracks[j].rect);
                if(!overlap.isZero())   //two track overlap
                {
                    int t1_layer = all_tracks[i].layer;
                    int t2_layer = all_tracks[j].layer;
                    if(t1_layer.direction==t2_layer.direction)
                    {
                        if(all_tracks[i].width>all_tracks[j].width)
                        {
                            Rectangle large_overlap = largeOverlap(overlap,all_tracks[i].width,t1_layer.direction);
                            Vertex vertex1(vertextId++,large_overlap.lower_left.x,large_overlap.lower_left.y,large_overlap.upper_right.x,large_overlap.upper_right.y,t1_layer);
                            vertexes.push_back(vertex1);
                            if(t1_layer!=t2_layer)
                            {
                                Vertex vertex2(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t2_layer);    
                                vertexes.push_back(vertex2);
                            }
                            
                        }else if(all_tracks[i].width<all_tracks[j].width)
                        {
                            Rectangle large_overlap = largeOverlap(overlap,all_tracks[j].width,t1_layer.direction);
                            Vertex vertex1(vertextId++,large_overlap.lower_left.x,large_overlap.lower_left.y,large_overlap.upper_right.x,large_overlap.upper_right.y,t2_layer);
                            vertexes.push_back(vertex1);
                            if(t1_layer!=t2_layer)
                            {
                                Vertex vertex2(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t1_layer);    
                                vertexes.push_back(vertex2);
                            }
                        }else
                        {
                            if(t1_layer==t2_layer)
                            {
                                Vertex vertex1(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t1_layer);
                                vertexes.push_back(vertex1);
                            }else
                            {
                                Vertex vertex1(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t1_layer);
                                Vertex vertex2(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t2_layer);
                                vertexes.push_back(vertex1);
                                vertexes.push_back(vertex2);
                            }
                        }
                    }else
                    {
                        Vertex vertex1(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t1_layer);
                        Vertex vertex2(vertextId++,overlap.lower_left.x,overlap.lower_left.y,overlap.upper_right.x,overlap.upper_right.y,t2_layer);
                        vertexes.push_back(vertex1);
                        vertexes.push_back(vertex2); 
                    }
                    
                }
            }
        }
    }
    Rectangle largeOverlap(Rectangle& overlap, double width, char direction)
    {
        Rectangle out;
        double a,b,c,d;
        a = overlap.lower_left.x;
        b = overlap.lower_left.y;
        c = overlap.upper_right.x;
        d = overlap.upper_right.y;
        if(direction=='H')
        {
            out = Rectangle(a,(b+d)/2-width/2,c,(b+d)/2+width/2);
        }else if(direction=='V')
        {
            out = Rectangle((a+c)/2-width/2,b,(a+c)/2-width/2,d);
        }
        return out;
    }

    void splitTrack(const Track& t, const Rectangle& overlap, std::stack<Track>& stack) {
        Layer layer = layers[t.layer];
        double a,b,c,d,e,f,g,h;
        a = t.rect.lower_left.x;
        b = t.rect.lower_left.y;
        c = t.rect.upper_right.x;
        d = t.rect.upper_right.y;
        e = overlap.lower_left.x;
        f = overlap.lower_left.y;
        g = overlap.upper_right.x;
        h = overlap.upper_right.y;
        if(layer.direction=='H'){
            if(a!=e)
            {
                Track t1(a,(b+d)/2,e,(b+d)/2,t.width,t.layer); //(a,b,e,d)
                stack.push(t1);
            }
            if(c!=g)
            {
                Track t2(g,(b+d)/2,c,(b+d)/2,t.width,t.layer); //(g,b,c,d)
                stack.push(t2);
            }
            if(b!=f)
            {
                Track t3(e,(b+f)/2,g,(b+f)/2,(double)(f-b),t.layer); //(e,b,g,f)
                stack.push(t3);
            }
            if(d!=h)
            {
                Track t4(e,(h+d)/2,g,(h+d)/2,(double)(d-h),t.layer); //(e,h,g,d)
                stack.push(t4);
            }

        }else if(layer.direction=='V')
        {
            if(b!=f)
            {
                Track t1((a+c)/2,b,(a+c)/2,f,t.width,t.layer); //(a,b,c,f)
                stack.push(t1);
            }
            if(d!=h)
            {
                Track t2((a+c)/2,h,(a+c)/2,d,t.width,t.layer); //(a,h,c,d)
                stack.push(t2);
            }
            if(a!=e)
            {
                Track t3((a+e)/2,f,(a+e)/2,h,(double)(e-a),t.layer); //(a,f,e,h)
                stack.push(t3);
            }
            if(c!=g)
            {
                Track t4((g+c)/2,f,(g+c)/2,h,(double)(c-g),t.layer); //(g,f,c,h)
                stack.push(t4);
            }
        }
    }

    std::vector<Layer>& layers;
    std::vector<Track>& tracks;
    std::vector<Bus>& buses;
    std::vector<Obstacle>& obstacles;
    std::vector<Vertex>& vertexes;
private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
