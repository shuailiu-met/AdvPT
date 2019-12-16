#include "Protoss.h"
#include <iostream>
#include <string>

Protoss::Protoss(std::vector<std::string> buildorder){
    // set race specific attributes
    energy = data->getAttributeValue("Nexus", DataAcc::start_energy, true);
    chrono_boost = 0;
    supply = data->getAttributeValue("Nexus", DataAcc::supply_provided, false);

    // build the starting unit
    for(int i = 0; i < data->getParameter("BASIS_START"); i++)
        finished.push_back(data->getUnit("Nexus"));
    for(int i = 0; i < data->getParameter("WORKERS_START"); i++){
        finished.push_back(data->getUnit("Probe"));
        workers++;
        supply_used += data->getAttributeValue("Probe", DataAcc::supply_cost, false);
    }
    // build buildings and add them to list
    for(std::vector<std::string>::iterator it = buildorder.begin(); it != buildorder.end(); it++) {
        Unit u = data->getUnit(*it);
        future.push_back(u);
    }

    worker_minerals = workers;
}

void Protoss::updateResources(){
    // TODO more than one building has energy
    // update energy of nexus
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    int energy_max = data->getAttributeValue("Nexus", DataAcc::max_energy, true);
    int energy_tmp = energy + energy_regen;
    energy = (energy_tmp > energy_max) ? energy_max : energy_tmp;

    // update vespene and minerals
    if(worker_minerals > 0){
        minerals += worker_minerals * data->getParameter("MINERAL_HARVESTING", true);
    }
    if(worker_vespene > 0){
        vespene += worker_vespene * data->getParameter("VESPENE_HARVESTING", true);
    }
}

void Protoss::advanceBuildingProcess(){
    // walk over all buildings and update build time
    std::vector<Unit> finishedTemp;
    for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
        it->updateTime(1 * FIXEDPOINT_FACTOR);

        // don't remove element while iterating over list
        if(it->isFinished()){
            finishedTemp.push_back(*it);
            // add supply if building provides it
            supply += data->getAttributeValue(it->getName(), DataAcc::supply_provided, false);
            // remove occupation
            Unit *occ = it->getOccupy();
            if(occ != nullptr){
                it->setOccupy(nullptr);
                occ->decOccupyBy();
            }
        }
    }

    // if finished elements -> move
    for(std::vector<Unit>::iterator it = finishedTemp.begin(); it != finishedTemp.end(); it++) {
        building.remove(*it);
        finished.push_back(*it);
        std::vector<int> i = {it->getId()};
        if(it->getName() == "Probe")
            workers++;
        if(it->getName() == "Assimilator")
            vespene_buildings += 1;
        addEvent("build-end", it->getName(), it->getBuildBy(), "", &i);
    }
}

int Protoss::specialAbility(){
    // TODO can this event be Added by other buildings
    // some more have energy!
    int cost = data->getParameter("CHRONOBOOST_ENERGY_COST", true);
    if(energy >= cost && chrono_boost == 0){
        energy -= cost;
        chrono_boost = data->getParameter("CHRONOBOOST_DURATION", false);
        //addEvent("special", "chronoboost", "Nexus_0", "Nexus_0");
        //return 1;
    }
    // TODO update chronoboosted units
    //if(chrono_boost > 0){
    //    it->updateTime(data->getParameter("CHRONOBOOST_SPEEDUP", true));
    if(chrono_boost > 0)
        chrono_boost --;
    return 0;
}

int Protoss::startBuildingProcess(){
    if(future.size() == 0)
        return 0;
    Unit newUnit = future.front();

    // check for resources
    int needMin = data->getAttributeValue(newUnit.getName(), DataAcc::minerals, true);
    int needVesp = data->getAttributeValue(newUnit.getName(), DataAcc::vespene, true);
    int needSupply = data->getAttributeValue(newUnit.getName(), DataAcc::supply_cost, false);
    // we don't meet the requirements
    if(needMin > minerals || needVesp > vespene || needSupply > (supply - supply_used)){
        // if we don't have enough supply and no buildings are buils -> cancel
        if(needSupply > (supply - supply_used) && building.size() == 0)
            return -2;
        return 0;
    }

    // check dependencies of new Unit
    std::vector<std::string> deps = data->getAttributeVector(newUnit.getName(), DataAcc::dependencies);
    // iterate over finished list to check if we have all the dependencies
    for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
        bool found = false;
        for(std::list<Unit>::iterator it2 = finished.begin(); it2 != finished.end(); it2++) {
            if(it2->getName() == *it)
                found = true;
        }
        // check if the needed dependency is currently build
        if(!found){
            for(std::list<Unit>::iterator it2 = building.begin(); it2 != building.end(); it2++) {
                if(it2->getName() == *it)
                    return 0;
            }
            // return a value which tells us to stop simulation
            return -2;
        }
    }

    // get producer of new Unit
    // TODO in protoss it seems that only one producer can exist
    // -> general case: multiple producers possible
    bool prodOK = false;
    bool prodExists = false;
    std::string prodName = "";
    std::string prod = data->getAttributeString(newUnit.getName(), DataAcc::producer);
    for(std::list<Unit>::iterator it = finished.begin(); it != finished.end(); it++) {
        if(prod == it->getName()){
            prodExists = true;
            std::string s = std::to_string(it->getId());
            prodName = it->getName() + "_" + s;
            newUnit.setBuildBy(prodName);
            // check if producer is occupied
            // TODO does 'producer_occupied' flag indicate that this building is
            // occupied when producing or that the production building of this building is occupied
            if(data->getAttributeString(newUnit.getName(), DataAcc::production_state) == "producer_not_occupied"){
                // we just need the buiding -> no need for occupation
                prodOK = true;
                break;
            }else if(data->getAttributeString(newUnit.getName(), DataAcc::production_state) == "producer_occupied"){
                // we need to register the producer and check for free capacity
                if(!it->isOccupied()){
                    it->incOccupiedBy();
                    newUnit.setOccupy(&(*it));
                    prodOK = true;
                    break;
                }
            }
        }
    }

    // if no producer in finished queue, search in building queue
    // TODO maybe only check for an empty building list, might be faster
    if(!prodExists){
        for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            if(prod == it->getName()){
                prodExists = true;
            }
        }
        // return value to indicate stopping of simulation
        if(!prodExists)
            return -2;
    }

    if(prodOK){
        // add event for JSON
        addEvent("build-start", newUnit.getName(), prodName);

        // manage lists
        building.push_back(newUnit);
        future.remove(newUnit);

        // manage materials
        minerals -= needMin;
        vespene -= needVesp;
        supply_used += needSupply;
        return 1;
    }
    return 0;
}
