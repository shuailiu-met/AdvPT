#include "Race.h"
#include "Protoss/Protoss.h"
#include "General/DataAcc.h"
#include <iostream>

int main(int argc, char* argv[]){
    // needs to be called with race and build parameter filepath
    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <race> <buildorder>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // TODO read buildorder file
    //placeholder:
    std::vector<std::string> buildorder {"Nexus", "Forge"};

    // get racename
    std::string race_name(argv[1]);

    // get the given race
    if(!race_name.compare("terran")){
        std::cout << "Use a Terran object!" << "\n";
    }else if(!race_name.compare("zerg")){
        std::cout << "Use a Zerg object!" << "\n";
    }else if(!race_name.compare("protoss")){
        std::cout << "Use a Protoss object!" << "\n";
        Protoss prot(buildorder);
        while(prot.advanceOneTimeStep());
    }else{
        std::cout << "Unknown race!" << std::endl;
        exit(EXIT_FAILURE);
    }


    return 0;
}
