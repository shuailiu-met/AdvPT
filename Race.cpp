#include "Race.h"
#include <iostream>

Race::Race(std::vector<std::string> buildings){
    // create DataAcc object
    data = new DataAcc("res/unit_db.csv", "res/parameters.csv");

    // set simulation time to zero
    currentTime = 0;

    // build buildings and add them to list
    for(std::vector<std::string>::iterator it = buildings.begin(); it != buildings.end(); it++) {
        Unit u = data->getUnit(*it);
        future.push_back(u);
    }

    // init resources
    workers = data->getParameter("WORKERS_START");
    minerals = data->getParameter("MINERALS_START");
    vespene = data->getParameter("VESPENE_START");
    supply = 0;
}


int Race::advanceOneTimeStep(){
    // Do all neccessary actions (see header file)

    // call Race specific function


    // just some testing stuff for now
    std::cout << "Workers: " << workers << ", Minerals: " << minerals;
    std::cout << ", Vespene: " << vespene << ", Supply: " << supply << "\n";

    for(std::list<Unit>::iterator it = future.begin(); it != future.end(); it++) {
        std::cout << (*it).getName() << "\n";
    }
    /*for (auto v : future)
        std::cout << v.getTimeLeft() << "\n";*/

    return 0;
}
