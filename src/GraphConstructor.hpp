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
        std::vector<std::vector<Track>> layerTracks(layers.size());
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
                    layerTracks[t.layer].push_back(t);
                }
            }
        }
        for (int i = 0; i < (int) layerTracks.size(); ++i) {
            for (const auto& track : layerTracks[i]) {
                for (const auto& track2 : layerTracks[i+1]) {
                    Rectangle overlap = track.rect.overlapWith(track2.rect);
                    if(!overlap.isZero())
                    {

                    }
                }
            }
        }
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
    std::vector<Vertex>& 
private:
    Logger& logger;
};

#endif // VLSI_FINAL_PROJECT_GRAPH_CONSTRUCTOR_HPP_
