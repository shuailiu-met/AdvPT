#include "Race.h"
#include "Protoss/Protoss.h"
#include "General/DataAcc.h"
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include <omp.h>

#define START_COUNT 10000

using namespace std;

struct optimize{
    std::vector<std::string> buildlist;
    int time;
    int count;
};

int main(int argc, char* argv[]){
    // needs to be called with race and build parameter filepath
    if(argc != 4){
        cout << "Usage: " << argv[0] << " [rush/push] <unit> <type-param>" << endl;
        exit(EXIT_FAILURE);
    }

    // seed the random number creation with an non-deterministic seed
    //srand(time(NULL));

    DataAcc data("res/unit_db.csv", "res/parameters.csv");

    // get parameter
    string strategy(argv[1]);
    string target_unit(argv[2]);
    int param = stoi(argv[3]);
    string race_name(data.getAttributeString(target_unit, DataAcc::race));

    int rush = (strategy == "rush") ? 1 : 0;

    // create random buildorders lists
    struct optimize arr[START_COUNT];

    int length;
    for(size_t i = 0; i < START_COUNT; i++){
        if(rush){
            // determine good length values depending on build time
            int build_time = data.getAttributeValue(target_unit, DataAcc::build_time);
            int min = (param / build_time) / 2;
            int max = (param / build_time) * 2;
            length = rand() % (max-min) + min;
        }else{
            length = param;
        }
        arr[i].buildlist = data.getRandomBuildorder(race_name, target_unit, strategy, length);
        arr[i].time = -1;
    }

    Race *sim;
    // get the given race
    if(!race_name.compare("Terr")){
        cout << "Use a Terran object!" << "\n";
    }else if(!race_name.compare("Zerg")){
        cout << "Use a Zerg object!" << "\n";
    }else if(!race_name.compare("Prot")){
        int finished = 0;
        while(!finished){
            finished = 1;
            #pragma omp parallel for private(sim)
            for(size_t i = 0; i < START_COUNT; i++){
                if(arr[i].time == -1){
                    sim = new Protoss(&data, arr[i].buildlist, false);
                    while((sim->advanceOneTimeStep()));
                    if(rush)
                        finished = 0;
                }else{
                    continue;
                }
                if((!rush && sim->buildlistValid()) ||
                        (rush && sim->buildlistValid() && sim->getSimulationTime() <= param)){
                    arr[i].count = count(arr[i].buildlist.begin(), arr[i].buildlist.end(),
                            target_unit);
                    arr[i].time = sim->getSimulationTime();
                }else{
                    if(rush && sim->buildlistValid() && sim->getSimulationTime() > param){
                        arr[i].buildlist.pop_back();
                        arr[i].time = -1;
                    }else{
                        //cout << "Invalid: " << sim->buildlistValid() << ", time: " << sim->getSimulationTime() << "\n";
                        arr[i].count = -1;
                        arr[i].time = -2;
                    }
                }
            }
        }
    }else{
        cout << "Unknown race!" << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> best;
    int best_count = -1;
    int best_time = 1000;
    if(rush){
        for(size_t i = 0; i < START_COUNT; i++){
            if(arr[i].count >= best_count){
                if(arr[i].count > best_count || best_time > arr[i].time){
                    best_count = arr[i].count;
                    best_time = arr[i].time;
                    best = arr[i].buildlist;
                }
            }
        }
    }else{
        for(size_t i = 0; i < START_COUNT; i++){
            if(arr[i].time > 0 && arr[i].time < best_time && arr[i].count == param){
                best_count = arr[i].count;
                best_time = arr[i].time;
                best = arr[i].buildlist;
            }
        }
    }

    /*cout << "Best List: \n";
      for(vector<string>::iterator it = best.begin(); it != best.end(); it ++)
      cout << *it << " ";
      cout << "\n";
      cout << "In time: " << best_time << "\n";*/
    sim = new Protoss(&data, best);
    while((sim->advanceOneTimeStep()));

    return 0;
}
