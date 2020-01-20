#include "Race.h"
#include "Protoss/Protoss.h"
#include "General/DataAcc.h"
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <cstdlib>
#include <ctime>

#define START_COUNT 10000

using namespace std;

struct optimize{
    int time;
    std::vector<std::string> buildlist;
    int count;
    int current_value;
};

int main(int argc, char* argv[]){
    // needs to be called with race and build parameter filepath
    if(argc != 4){
        cout << "Usage: " << argv[0] << " [rush/push] <unit> <type-param>" << endl;
        exit(EXIT_FAILURE);
    }

    // seed the random number creation with an non-deterministic seed
    srand(time(NULL));

    DataAcc data("res/unit_db.csv", "res/parameters.csv");

    // get parameter
    string target_unit(argv[2]);
    string race_name(data.getAttributeString(target_unit, DataAcc::race));

    // create random buildorders lists
    struct optimize arr[START_COUNT];
    for(size_t i = 0; i < START_COUNT; i++){
        arr[i].buildlist = data.getRandomBuildorder(race_name, target_unit, 10);
    }

    Race *simRace = nullptr;
    // get the given race
    if(!race_name.compare("Terr")){
        cout << "Use a Terran object!" << "\n";
    }else if(!race_name.compare("Zerg")){
        cout << "Use a Zerg object!" << "\n";
    }else if(!race_name.compare("Prot")){
        for(size_t i = 0; i < START_COUNT; i++){
            simRace = new Protoss(&data, arr[i].buildlist, false);
            while((simRace->advanceOneTimeStep()));
            cout << "Valid: " << simRace->buildlistValid() << ", time: " << simRace->getSimulationTime() << "\n";
        }
    }else{
        cout << "Unknown race!" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Finish!!!" << "\n";
    return 0;
}
