#ifndef VLSI_FIANL_PROJECT_LAYER_HPP_
#define VLSI_FIANL_PROJECT_LAYER_HPP_
#include <string>
#include <map>
class Layer{
public:
    Layer(){
        all_layer.clear();
        cur_layer_count=0;
    }
    int cur_layer_count;
    map<string,int> all_layer;
    void addLayer(std::string s)
    {
        all_layer[string]=cur_layer_count;
        cur_layer_count++;
    }
    int getLayerID(std::string s)
    {
        return all_layer[s];
    }
    void cleanAll()
    {
        all_layer.clear();
    }

};
#endif