#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <sys/stat.h>
#include <fstream>
#include "Rectangle.hpp"
#include "Logger.hpp"
#include "InputReader.hpp"
#include "Point.hpp"
#include "Bit.hpp"
#include "Pin.hpp"
#include "DisjointSet.hpp"

class OutBus{
public:
    OutBus(){}
    void clearBus(){
        bitName.clear();
        bitsToPath.clear();
    }
    std::string name;
    std::vector<int> widths;
    std::vector<std::string> bitName;
    std::map<std::string,std::vector<Track>> bitsToPath;
};

void outputRead(std::vector<OutBus>& outBuses);

bool doubleEqual(const double& a,const double& b){
    return (fabs(a-b)<1e-6) ? true : false;
}

bool isLessEqual(const double& a,const double& b){
    if(a<b||doubleEqual(a,b)) return true;
    else return false;
}

bool pointOverlap(Track& a,Track& b){
    if(doubleEqual(a.rect.width,0)&&doubleEqual(a.rect.height,0) &&
       doubleEqual(b.rect.width,0)&&doubleEqual(b.rect.height,0)){
        /// a b are via
        if(doubleEqual(a.terminal[0].x,b.terminal[0].x)&&doubleEqual(a.terminal[0].y,b.terminal[0].y))
            return true;
    }else if(doubleEqual(a.rect.width,0)&&doubleEqual(a.rect.height,0)){
        ///a is a via
        if(isLessEqual(a.rect.ll.x,b.rect.ur.x)&&isLessEqual(a.rect.ll.y,b.rect.ur.y)){
            if(isLessEqual(b.rect.ll.x,a.rect.ll.x)&&isLessEqual(b.rect.ll.y,a.rect.ll.y)){
                return true;
            }
        }
    }else if(b.rect.width<1e-6&&b.rect.height<1e-6){
        ///b is a via
        if(isLessEqual(b.rect.ll.x,a.rect.ur.x)&&isLessEqual(b.rect.ll.y,a.rect.ur.y)){
            if(isLessEqual(a.rect.ll.x,b.rect.ll.x)&&isLessEqual(a.rect.ll.y,b.rect.ll.y)){
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
                outBus.widths = curBus.widths;
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
                            for(unsigned int k=0;k<layers.size();k++){
                                if(layers[k].name.compare(layerName)==0) layer = k;
                            }
                            double width = curBus.widths[layer];
                            //std::cout<<"layer "<<layer<<" width "<<width<<"\n";
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

bool TrackToAllBuses(std::vector<OutBus>& outBuses,Track& curTrack,int curBus, int curBit,int curTck){
    printf("\ncheckingBus: %d, checkingBit: %d checkingTrack:%d\n",curBus,curBit,curTck);
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            if(i==curBus&&j==curBit){
                continue;
            }
            printf("curBus: %d, curBit: %d\n",i,j);
            std::string& bitName = curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            for(unsigned int k =0;k<curPath.size();k++){
                if(curTrack.hasOverlap(curPath[k],true)&&curTrack.layer==curPath[k].layer){
                    printf("Overlap : curBus: %d, curBit: %d, curTrack: %d\n",i,j,k);
                    return true;
                }else if(pointOverlap(curTrack,curPath[k])&&abs(curTrack.layer-curPath[k].layer)<=1){
                    printf("Overlap : curBus: %d, curBit: %d, curTrack: %d\n",i,j,k);
                    return true;
                }
            }
        }
    }
    return false;
}


bool checkPathOverlap(std::vector<OutBus>& outBuses){
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            for(unsigned int k =0;k<curPath.size();k++){
                Track& curTrack = curPath[k];
                bool isOverlap = TrackToAllBuses(outBuses,curTrack,i,j,k);
                if(isOverlap==true){
                    printf("Some track is Overlap with different bit's track\n");
                    return false;
                }
            }
        }
    }
    printf("No track is Overlap with different bit's track\n");
    return true;
}

bool checkPathConnect(std::vector<OutBus>& outBuses){
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            DisjointSet DisSet(curPath.size());

            for(unsigned int n =0;n<curPath.size();n++){
                Track & curTrack = curPath[n];
                for(unsigned int m =0;m<curPath.size();m++){
                    if(m==n){
                        continue;
                    }
                    if(curTrack.hasOverlap(curPath[m],true)&&curTrack.layer==curPath[m].layer){
                        DisSet.pack(m,n);
                    }else if(pointOverlap(curTrack,curPath[m])&&abs(curTrack.layer-curPath[m].layer)<=1){
                        DisSet.pack(m,n);
                    }
                }
            }

            if(DisSet.numGroups()==1) printf("Bit %d is connected\n",j);
            else printf("Bit %d is not connected\n",j);

        }
    }

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

double calHPWL(std::vector<Track>& path){
    double maxX = path[0].rect.ur.x, maxY = path[0].rect.ur.y,
           minX = path[0].rect.ll.x, minY = path[0].rect.ll.x;

    for(unsigned int i=1;i<path.size();i++){
        if(path[i].rect.ur.x>maxX) maxX = path[i].rect.ur.x;
        if(path[i].rect.ur.y>maxY) maxY = path[i].rect.ur.y;
        if(path[i].rect.ll.x<minX) minX = path[i].rect.ll.x;
        if(path[i].rect.ll.y<minY) minY = path[i].rect.ll.y;
    }
    return maxX-minX+maxY-minY;
}

double calCwi(std::vector<OutBus>& outBuses,double alpha){
    double cost = 0;
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        double Cwi = 0;
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            double wl = 0;
            for(unsigned int n =0;n<curPath.size();n++){
                double pathLength = fabs(curPath[n].terminal[0].x-curPath[n].terminal[0].y) +
                                    fabs(curPath[n].terminal[0].y-curPath[n].terminal[0].y) ;
                wl = wl + pathLength;
            }
            double HPWL = calHPWL(curPath);
            Cwi = Cwi + wl/HPWL;
        }
        cost = cost + alpha*(Cwi/(double) curOutBus.bitName.size());
    }
    return cost;
}

char trackDir(Track& track){
    if(track.rect.isZero()) return 'X';
    if(doubleEqual(track.terminal[0].x,track.terminal[1].x)) return 'H';
    return 'V';
}

double calCsi(std::vector<OutBus>& outBuses, double beta){
    double cost = 0;
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        double Csi = 0;
        int totalSeg = 0;
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            int seqCount = 0;
            char prevDir = '\0';
            for(unsigned int k=0;k<curPath.size();k++){
                char dir = trackDir(curPath[k]);
                if(dir==' '){

                }
                else if(dir=='X'){
                    /// via no cost needed
                }else if(prevDir=='\0'){
                    seqCount = seqCount + 1;
                    prevDir = dir;
                }else if(prevDir == dir){
                    ///same direction no cost needed
                }else if(prevDir != dir){
                    seqCount = seqCount + 1;
                    prevDir = dir;
                }
            }
            totalSeg = totalSeg + seqCount;

        }
        cost = cost + beta*totalSeg;
    }
    return cost;
}

double calCci(std::vector<OutBus>& outBuses,double gamma,std::vector<Layer> layers){

    double cost = 0;
    for(unsigned int i=0;i<outBuses.size();i++){
        OutBus& curOutBus = outBuses[i];
        double Cci = 0;
        int segCount = 0;
        std::vector<std::vector<Track>> curSeqs;
        for(unsigned int j=0;j<curOutBus.bitName.size();j++){
            std::string& bitName =curOutBus.bitName[j];
            std::vector<Track>& curPath = curOutBus.bitsToPath[bitName];
            char prevDir = '\0';
            std::vector<Track> seq;
            //std::cout<<curPath.size()<<std::endl;
            for(unsigned int k=0;k<curPath.size();k++){
                char dir = trackDir(curPath[k]);
                //std::cout<<k<<"<"<<curPath.size()<<" "<<dir<<" ";
                if(dir==' '){

                }
                else if(dir=='X'){
                    /// via no cost needed
                }else if(prevDir=='\0'){
                    seq.emplace_back(curPath[k]);
                    prevDir = dir;
                }else if(prevDir == dir){
                    ///same direction no cost needed
                }else if(prevDir != dir){
                    seq.emplace_back(curPath[k]);
                    prevDir = dir;
                }
            }
            //std::cout<<"***********"<<std::endl;
            curSeqs.emplace_back(seq);
        }


        for(unsigned int j=0;j<curSeqs[0].size();j++){
            double maxC, minC;
            bool maxI = false, minI = false;
            for(unsigned int k=0;k<curSeqs.size();k++){
                std::vector<Track> &seq = curSeqs[k];
                if(trackDir(seq[j])=='H'){
                    /// y same
                    if(maxC<seq[j].terminal[0].y || maxI == false){
                        maxC = seq[j].terminal[0].y;
                        maxI = true;
                    }
                    if(minC>seq[j].terminal[0].y || minI == false){
                        minC = seq[j].terminal[0].y;
                        minI = true;
                    }
                }
                if(trackDir(seq[j])=='V'){
                    /// x same
                    if(maxC<seq[j].terminal[0].x || maxI == false){
                        maxC = seq[j].terminal[0].x;
                        maxI = true;
                    }
                    if(minC>seq[j].terminal[0].x || minI == false){
                        minC = seq[j].terminal[0].x;
                        minI = true;
                    }
                }
            }
            double SeqWidth = curOutBus.widths[curSeqs[0][j].layer];
            double seqSapcing = layers[curSeqs[0][j].layer].spacing;
            SeqWidth = SeqWidth * curOutBus.bitName.size()-1;
            double minimumCost = SeqWidth+seqSapcing;
            cost = cost + (maxC-minC)/minimumCost;
        }
        cost = cost/(double) curSeqs[0].size();

    }
    return cost;
}

double cal_cost(std::vector<OutBus>& outBuses,std::vector<Layer>& layers,double alpha, double beta,double gamma){
    return calCwi(outBuses,alpha)+calCsi(outBuses,beta)+calCci(outBuses,gamma,layers);
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
    int alpha, beta, gamma;


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
        alpha = inputReader.alpha;
        beta = inputReader.beta;
        gamma = inputReader.gamma;
    }

    ///read output file
    std::vector<OutBus> outBuses;
    outBuses = readOutput(outputFile,buses,layers);
    //outputRead(outBuses);



    //checkPinConnect(outBuses,buses);
    checkPathConnect(outBuses);
    //checkPathOverlap(outBuses);
    std::cout<<"cost ["<<cal_cost(outBuses,layers,alpha,beta,gamma)<<"]";

}
