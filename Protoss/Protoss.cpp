#include "Protoss.h"
#include <iostream>
#include <string>

Protoss::Protoss(DataAcc *dat, std::vector<std::string> buildorder, bool sim) : Race(dat, sim){
    // set race specific attributes
    supply = data->getAttributeValue("Nexus", DataAcc::supply_provided, false);

    // build the starting unit
    for(int i = 0; i < data->getParameter("BASIS_START"); i++){
        Unit u = data->getUnit("Nexus");
        finished.push_back(u);
        hasEnergy.push_back(u);
        base_buildings += 1;
    }
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
    vesp_cur_build = 0;
}

void Protoss::updateResources(){
    // update energy of nexus
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    for(std::list<Unit>::iterator it = hasEnergy.begin(); it != hasEnergy.end(); it ++){
        it->setEnergy(energy_regen);
    }
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
        if(chronoboost[it->getBuildBy()]){
            it->updateTime(data->getParameter("CHRONOBOOST_SPEEDUP", true));
        }else{
            it->updateTime(1 * FIXEDPOINT_FACTOR);
        }
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

        if(it->getName() == "Probe"){
            workers++;
        }else if(it->getName() == "Assimilator"){
            vesp_cur_build -= 1;
            vespene_buildings += 1;
        }else if(it->getName() == "Nexus"){
            base_buildings += 1;
            hasEnergy.push_back(*it);
        }
        addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
    }
}

int Protoss::specialAbility(){
    // TODO what is the best way of using this?
    // - multiple nexus possible
    // - casting on which building or Unit
    // - all building processes by this building are faster?
    // - can it be cast multiple times qhile its still running?
    std::unordered_map<std::string, int>::iterator it;
    for(it = chronoboost.begin(); it != chronoboost.end(); it ++){
        if(chronoboost[it->first] > 0){
            chronoboost[it->first] = chronoboost[it->first] - 1;
        }
    }

    int cost = data->getParameter("CHRONOBOOST_ENERGY_COST", true);
    for(std::list<Unit>::iterator it = hasEnergy.begin(); it != hasEnergy.end(); it ++){
        // chrono boost can be activated
        std::string id = it->getName() + "_" + std::to_string(it->getId());
        if(it->currentEnergy() >= cost && chronoboost[id] == 0){
            // TODO for the time being just activate it on itself
            chronoboost[id] = data->getParameter("CHRONOBOOST_DURATION", false);
            it->setEnergy(-cost);
            addEvent("special", "chronoboost", id, id);
            return 1;
        }
    }
    return 0;
}

int Protoss::startBuildingProcess(){
    if(future.size() == 0)
        return 0;
    Unit newUnit = future.front();

    // check if Unit is from the correct race
    if(data->getAttributeString(newUnit.getName(), DataAcc::race) != "Prot"){
        //std::cout << "Wrong Race!!!\n";
        return -2;
    }

    // no more than 2 vespene buildings per base
    if(newUnit.getName() == "Assimilator" && ((vespene_buildings + vesp_cur_build) >= 2 * base_buildings)){
        // check if base is currentliy building
        for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            // found -> wait for buildingd to be finished
            if("Nexus" == it->getName()){
                return 0;
            }
        }
        // else -> stop simulation
        return -2;
    }

    // check for resources
    int needMin = data->getAttributeValue(newUnit.getName(), DataAcc::minerals, true);
    int needVesp = data->getAttributeValue(newUnit.getName(), DataAcc::vespene, true);
    int needSupply = data->getAttributeValue(newUnit.getName(), DataAcc::supply_cost, false);
    // we don't meet the requirements
    if(needMin > minerals || needVesp > vespene || needSupply > (supply - supply_used)){
        // if no vespene buildings, return -2
        if(needVesp > vespene && building.size() == 0 && vespene_buildings == 0){
            std::cout << "No vespene!!!\n";
            return -2;
        }
        // if we don't have enough supply and no buildings are build -> cancel
        if(needSupply > (supply - supply_used) && building.size() == 0){
            std::cout << "Supply " << supply << " Used " << supply_used << "\n";
            return -2;
        }
        return 0;
    }

    // TODO dependencies are 'or' because of upgrades not 'and'
    // => implement (might not be important for protoss)
    // check dependencies of new Unit
    std::vector<std::string> deps = data->getAttributeVector(newUnit.getName(), DataAcc::dependencies);
    // iterate over finished list to check if we have the dependencies
    for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
        bool found = false;
        for(std::list<Unit>::iterator it2 = finished.begin(); it2 != finished.end(); it2++) {
            if(it2->getName() == *it){
                found = true;
                break;
            }
        }
        // check if the needed dependency is currently build
        if(!found){
            for(std::list<Unit>::iterator it2 = building.begin(); it2 != building.end(); it2++) {
                if(it2->getName() == *it)
                    return 0;
            }
            // return a value which tells us to stop simulation
            std::cout << "Dependency not met!!! \"" << *it <<"\" for \"" << newUnit.getName() << "\" needed!!\n";
            return -2;
        }else{
            break;
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
            // check if producer will be occupied
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
            }else{
                assert(false);
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
        if(!prodExists){
            std::cout << "Producer doesn't exist!!!\n";
            return -2;
        }
    }

    if(prodOK){
        if(newUnit.getName() == "Assimilator")
            vesp_cur_build += 1;
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
