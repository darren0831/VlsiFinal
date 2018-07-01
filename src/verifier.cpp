#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <sys/stat.h>
#include "Rectangle.hpp"
#include "Logger.hpp"
#include "InputReader.hpp"
#include "Point.hpp"
#include "Bit.hpp"
#include "Pin.hpp"

class OutBus{
public:
    OutBus(){}
    void clearBus(){
        bitName.clear();
        bitsToPath.clear();
    }
    std::string name;
    std::vector<std::string> bitName;
    std::map<std::string,std::vector<Track>> bitsToPath;
};

void outputRead(std::vector<OutBus>& outBuses);

bool doubleEqual(const double& a,const double& b){
    return (fabs(a-b)<1e-6) ? true : false;
}

bool pointOverlap(Track& a,Track& b){
    if((a.rect.width<1e-6&&a.rect.height<1e-6)&&(b.rect.width<1e-6&&b.rect.height<1e-6)){
        /// a b are via
        if(doubleEqual(a.terminal[0].x,b.terminal[0].x)&&doubleEqual(a.terminal[0].y,b.terminal[0].y))
            return true;
    }else if(a.rect.width<1e-6&&a.rect.height<1e-6){
        ///a is a via
        if((b.rect.ur.x>a.rect.ll.x||doubleEqual(b.rect.ur.x,a.rect.ll.x))&&(b.rect.ur.y>a.rect.ll.y||doubleEqual(b.rect.ur.y,a.rect.ll.y))){
            if((b.rect.ll.x<a.rect.ll.x||doubleEqual(b.rect.ll.x,a.rect.ll.x))&&(b.rect.ll.y<a.rect.ll.y||doubleEqual(b.rect.ll.y,a.rect.ll.y))){
                return true;
            }
        }
    }else if(b.rect.width<1e-6&&b.rect.height<1e-6){
        ///b is a via
        if((a.rect.ur.x>b.rect.ll.x||doubleEqual(a.rect.ur.x,b.rect.ll.x))&&((a.rect.ur.y>b.rect.ll.y)||doubleEqual(a.rect.ur.y,b.rect.ll.y))){
            if((a.rect.ll.x<b.rect.ll.x||doubleEqual(a.rect.ll.x,b.rect.ll.x))&&(a.rect.ll.y<b.rect.ll.y||doubleEqual(a.rect.ll.y,b.rect.ll.y))){
                return true;
            }
        }
    }
    return false;
}



char LayerDir(std::string name,std::vector<Layer> layers){
    for(unsigned int i=0;i<layers.size();i++){
        if(name.compare(layers[i].name)==0){
           return layers[i].direction;
        }
    }
}

std::vector<OutBus> readOutput(std::string filename,std::vector<Bus>& buses,std::vector<Layer> layers){
    std::ifstream outfile(filename);
    std::string line, bitName;
    std::vector<OutBus> outBuses;

    if(outfile.is_open()){
        while (outfile>>line){
            if(line.compare("BUS")==0){
                OutBus outBus;
                outfile>>line;
                outBus.name = line;
                int curBusId = 0;
                for(unsigned int i=0;i<buses.size();i++){
                    if(outBus.name.compare(buses[i].name)==0) curBusId = i;
                }
                Bus& curBus = buses[curBusId];
                while(true){
                    outfile>>line;
                    if(line.compare("BIT")==0){
                        outfile>>line;
                        bitName = line;
                        //std::cout<<"BIT "<<line<<std::endl;
                        outBus.bitName.emplace_back(line);
                    }else if(line.compare("PATH")==0){
                        outfile>>line;
                        //std::cout<<"PATH "<<line<<std::endl;
                        int numPath = std::stoi(line);
                        std::vector<Track> singlePath(numPath);
                        bool layerIsReaded = false;
                        for(int i=0;i<numPath;i++){
                            if(layerIsReaded == false) outfile>>line;
                            else layerIsReaded = false;
                            int layer;
                            std::string layerName = line;
                            //std::cout<<"layer "<<line<<" ";
                            for(unsigned int j=0;j<line.size();j++){
                                if(isdigit(line[j])){
                                    layer = std::stoi(line.substr(j));
                                    break;
                                }
                            }
                            double width = curBus.widths[layer-1];
                            std::cout<<"layer "<<layer<<" width "<<width<<"\n";
                            Point ll = Point(0,0);
                            outfile>>line;
                            ll.x = std::stod(line.substr(1));
                            outfile>>line;
                            ll.y = std::stod(line.substr(0,line.size()-1));

                            outfile>>line;
                            if(line[0]!='('){
                                /// a via
                                Track temp = Track(ll.x,ll.y,ll.x,ll.y,0,layer);
                                singlePath[i] = temp;
                                layerIsReaded = true;
                            }else{
                                /// a path
                                Point ur = Point(0,0);
                                ur.x = std::stod(line.substr(1));
                                outfile>>line;
                                ur.y = std::stod(line.substr(0,line.size()-1));
                                Track temp = Track(ll.x,ll.y,ur.x,ur.y,width,layer);
                                singlePath[i] = temp;
                                layerIsReaded = false;
                            }
                            //std::cout<<"( "<<singlePath[i].rect.ll.x<<" "<<singlePath[i].rect.ll.y<<" ) ";
                            //std::cout<<"( "<<singlePath[i].rect.ur.x<<" "<<singlePath[i].rect.ur.y<<" )\n";
                        }
                        outBus.bitsToPath[bitName] = singlePath;
                    }else if(line.compare("ENDPATH")==0 || line.compare("ENDBIT") ==0){

                    }else if(line.compare("ENDBUS")==0){
                        outBuses.emplace_back(outBus);
                        outBus.clearBus();
                        break;
                    }
                }
            }
        }
    }else{
        printf("fail to open file\n");
    }
    return outBuses;
}

void outputRead(std::vector<OutBus>& outBuses){
    std::cout<<"\n***check output***"<<std::endl;
    //std::cout<<"Bus num: "<<outBuses.size()<<std::endl;
    for(unsigned int k = 0;k<outBuses.size();k++){
        OutBus& curBus = outBuses[k];
        std::cout<<"BUS "<<curBus.name<<"\n";
        //std::cout<<"BIT num: "<<curBus.bitName.size()<<std::endl;
        for(unsigned int i=0;i<curBus.bitName.size();i++){
            std::string& curBit = curBus.bitName[i];
            std::cout<<"BIT "<<curBit<<"\n";
            std::vector<Track> & curPath = curBus.bitsToPath[curBit];
            std::cout<<"Path "<<curPath.size()<<std::endl;
            for(unsigned int j=0;j<curPath.size();j++){
                Track& curTrack = curPath[j];
                if(fabs(curTrack.rect.ll.x-curTrack.rect.ur.x)>1e-6 || fabs(curTrack.rect.ll.y-curTrack.rect.ur.y)>1e-6){
                    printf("L%d ( %.1f %.1f ) ( %.1f %.1f )\n",curTrack.layer,curTrack.rect.ll.x,curTrack.rect.ll.y,curTrack.rect.ur.x,curTrack.rect.ur.y);
                }else{
                    printf("L%d ( %.1f %.1f ) \n",curTrack.layer,curTrack.rect.ll.x,curTrack.rect.ll.y);
                }
            }
            printf("ENDPATH\n");
        }
        printf("ENDBUS\n");
    }


}

bool checkOverlap(std::vector<std::vector<Track>>& paths){
    return true;
}

bool checkPathConnect(std::vector<OutBus>& outBuses){
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            bool isTouched[curPath.size()-1];
            for(unsigned int k=0;k<curPath.size();k++) isTouched[k] = false;

            for(unsigned int n =0;n<curPath.size();n++){
                Track & curTrack = curPath[n];
                for(unsigned int m =0;m<curPath.size();m++){
                    if(m==n){
                        continue;
                    }
                    if(curTrack.hasOverlap(curPath[m],true)){
                        isTouched[n] = true;
                        break;
                    }else if(pointOverlap(curTrack,curPath[m])){
                        isTouched[n] = true;
                        break;
                    }
                }
            }

            for(unsigned int k=0;k<curPath.size();k++){
                if(isTouched[k]==false){
                    printf("Path is not connected\n");
                    return false;
                }
            }
        }
    }
    printf("Path is connected\n");
    return true;
}

bool checkPinConnect(std::vector<OutBus>& outBuses,std::vector<Bus>& buses){
    ///check whether all bits is connect to path
    for(unsigned int i=0;i<buses.size();i++){
        for(unsigned int j=0;j<outBuses.size();j++){
            /// find coordinate Bus
            if(outBuses[j].name.compare(buses[i].name)==0){
                OutBus& curOutBus = outBuses[j];
                Bus& curBus = buses[i];
                /// for every pin in every bit, check connection
                for(unsigned int k=0;k<curBus.bits.size();k++){
                    Bit& curBit = curBus.bits[k];
                    std::vector<Track>& curPath = curOutBus.bitsToPath[curBit.name];
                    ///check if every pin is overlap with one Track
                    for(unsigned int m=0;m<curBit.pins.size();m++){
                        Pin& curPin = curBit.pins[m];
                        bool found = false;
                        for(unsigned int n=0;n<curPath.size();n++){
                            if(curPath[n].rect.hasOverlap(curPin.rect,true)){
                                found = true;
                                break;
                            }
                        }
                        if(!found){
                            printf("bit is not connect to track!!\n");
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

int main(int argc,char **argv){

    if (argc != 3) {
        fprintf(stderr, "usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    std::string inputfile(argv[1]);
    std::string outputFile(argv[2]);


    /// Input information
    Logger stdoutLogger;
    Logger inputLogger("Log/input.log");
    std::vector<Layer> layers;
    std::vector<Track> tracks;
    std::vector<Bus> buses;
    std::vector<Obstacle> obstacles;
    Rectangle boundary;

    /// Read input file
    stdoutLogger.info("===== Read Input File =====\n");
    {
        Logger& logger = stdoutLogger;
        InputReader inputReader(inputfile, logger);
        layers = std::move(inputReader.layers);
        tracks = std::move(inputReader.tracks);
        buses = std::move(inputReader.buses);
        obstacles = std::move(inputReader.obstacles);
        boundary = std::move(inputReader.boundary);
    }

    ///read output file
    std::vector<OutBus> outBuses;
    outBuses = readOutput(outputFile,buses,layers);
    outputRead(outBuses);


    //checkPinConnect(outBuses,buses);
    checkPathConnect(outBuses);

}
