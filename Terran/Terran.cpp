//
//  Terran.cpp
//  SC2-4
//
//  Created by 宋經緯 on 2019/12/15.
//  Copyright © 2019 宋經緯. All rights reserved.
//
#include "Terran.h"
#include <iostream>
#include <string>


Terran::Terran(std::vector<std::string> buildorder){
    supply = data->getAttributeValue("CommandCenter", DataAcc::supply_provided, false);

    // build the starting unit
    for(int i = 0; i < data->getParameter("BASIS_START"); i++)
           finished.push_back(data->getUnit("CommandCenter"));
       for(int i = 0; i < data->getParameter("WORKERS_START"); i++){
           finished.push_back(data->getUnit("SCV"));
           workers++;
           supply_used += data->getAttributeValue("SCV", DataAcc::supply_cost, false);
       }
    
    // build buildings and add them to list
    for(std::vector<std::string>::iterator it = buildorder.begin(); it != buildorder.end(); it++) {
        Unit u = data->getUnit(*it);
        future.push_back(u);
    }
    mule_duration = 64;
    duration = 0;
    worker_minerals = workers;
}


void Terran::updateResources(){
//    int mule_energy_cost = data->getParameter("MULE_ENERGY_COST",true);
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    if(worker_minerals > 0){
           minerals += worker_minerals * data->getParameter("MINERAL_HARVESTING", true);
//        std::cout<<worker_minerals<<std::endl;
       }
    if(worker_vespene > 0){
           vespene += worker_vespene * data->getParameter("VESPENE_HARVESTING", true);
    }
    for(std::list<Unit>::iterator it = hasEnergy.begin(); it != hasEnergy.end(); it ++){
        it->setEnergy(energy_regen);
    }
}

void Terran::addspecialability(){
//            when durationtime haven't ended,continue add on
        if (releasemule) {
            if (duration != mule_duration) {
                minerals += 0.7*data->getParameter("MULE_SPEEDUP", true);
    //            std::cout<< minerals<<std::endl;
                duration++;
            }else if (duration == mule_duration) {
                duration = 0;
                releasemule = false;
            }
        }
}

void Terran::advanceBuildingProcess(){
      // walk over all buildings and update build time
        std::vector<Unit> finishedTemp;
        for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            it->updateTime(1 * FIXEDPOINT_FACTOR);

            // don't remove element while iterating over list
            if(it->isFinished()){
                finishedTemp.push_back(*it);
//                Unit newUnit =finishedTemp.front();
                // add supply if building provides it
//                if (it->getName() == "OrbitalCommand") {
//                    std::list<Unit>::iterator it1 = finished.begin();
//                        finished.remove(*it1);
//                }
//                std::cout<< it->getName()<<std::endl;//cout the finised thing
                //if building contain supply
                if (it->getName() != "OrbitalCommand") {
                    if (it->getName() != "PlanetaryFortress") {
                supply += data->getAttributeValue(it->getName(), DataAcc::supply_provided, false);
                }
                }
                
                // remove occupation
                Unit *occ = it->getOccupy();
                if(occ != nullptr){
                    it->setOccupy(nullptr);
                    occ->decOccupyBy();
                }
                
                //upgrade building
                if(data->getAttributeString(it->getName(), DataAcc::production_state) == "producer_consumed_at_end" ){
                    std::string prod = data->getAttributeString(it->getName(), DataAcc::producer);
                    for(std::list<Unit>::iterator it1 = finished.begin(); it1 !=finished.end(); it1++){
                        if (it1->getName() == prod) {
                            finished.remove(*it1);
                            break;
                        }
                        
                    }
                }
//                if (it->getName() == "OrbitalCommand") {
//                    std::list<Unit>::iterator it1 = finished.begin();
//                        finished.remove(*it1);
//                }
//                for (std::list<Unit>::iterator it1 = finished.begin(); it1 != finished.end(); it1++) {
//                    if (it1->getName() == "OrbitalCommand")
//                     finished.remove(*it);
//                    if (it->getName() == "PlanetaryFortress")
//                        finished.remove(*it);
//
//                }
            }
        }

        // if finished elements -> move
        for(std::vector<Unit>::iterator it = finishedTemp.begin(); it != finishedTemp.end(); it++) {
            building.remove(*it);
            finished.push_back(*it);
            
            std::vector<std::string> prod = data->getAttributeVector(it->getName(), DataAcc::producer);
            for(std::vector<std::string>::iterator it = prod.begin(); it != prod.end(); it++) {
                if (*it == "SCV") {
                    occupyingworker--;
                    }
                }
            std::vector<int> i = {it->getId()};
            if(it->getName() == "SCV")
                workers++;
            if(it->getName() == "Refinery")
                vespene_buildings += 1;
            if(it->getName() == "OrbitalCommand")
                       hasEnergy.push_back(*it);
            addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
        }
    
}

int Terran::specialAbility(){
    // TODO can this event be Added by other buildings
    // some more have energy!
    
    addspecialability();
    
    std::unordered_map<std::string, int>::iterator it;
    for(it = mule.begin(); it != mule.end(); it ++){
        if(mule[it->first] > 0){
             mule[it->first] = mule[it->first] - 1;
        }
    }

    int cost = data->getParameter("MULE_ENERGY_COST", true);
    for(std::list<Unit>::iterator it = hasEnergy.begin(); it != hasEnergy.end(); it ++){
        // mule speedup can be activated
        std::string id = it->getName() + "_" + std::to_string(it->getId());
//        std::cout<< it->currentEnergy()<<std::endl;
        if(it->currentEnergy() >= cost && mule[id] == 0){
            // TODO for the time being just activate it on itself
            mule[id] = data->getParameter("MULE_DURATION", false);
            it->setEnergy(-cost);
            releasemule = true;
            addEvent("special", "mule", id, id);
            return 1;
        }
    }
    return 0;
}



int  Terran::startBuildingProcess(){
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
        if (deps.size() != 0) {
           bool found = false;
            for(std::list<Unit>::iterator it1 = finished.begin(); it1 != finished.end(); it1++) {
               if (deps.size() == 3) {
                   if (it1->getName() == deps[0]||it1->getName() == deps[1]||it1->getName() == deps[2])
                       found = true;
               }
               else if (deps.size() != 3) {
                   if(it1->getName() == deps[0])
                       found = true;
              }
           }
           if(!found){
               for(std::list<Unit>::iterator it2 = building.begin(); it2 != building.end(); it2++) {
                if (deps.size() == 3) {
                    if (it2->getName() == deps[0]||it2->getName() == deps[1]||it2->getName() == deps[2])
                       return 0;
                    }
                else if(deps.size() != 3){
                    if (it2->getName() == deps[0]) {
                        return 0;
                    }
                }
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
       std::vector<std::string> prod = data->getAttributeVector(newUnit.getName(), DataAcc::producer);
       for (std::vector<std::string>::iterator vec_prod = prod.begin(); vec_prod != prod.end();
            vec_prod++) {
        for(std::list<Unit>::iterator it = finished.begin(); it != finished.end(); it++) {
             if( *vec_prod == it->getName()){
               prodExists = true;
               std::string s = std::to_string(it->getId());
                 if (!it->isOccupied()) {
               prodName = it->getName() + "_" + s;
                 }
               newUnit.setBuildBy(prodName);
               // check if producer is occupied
               // TODO does 'producer_occupied' flag indicate that this building is
               // occupied when producing or that the production building of this building is occupied
//               if(data->getAttributeString(newUnit.getName(), DataAcc::production_state) == "producer_consumed_at_end" ){
                if(!it->isOccupied()){
                   it->incOccupiedBy();
                   newUnit.setOccupy(&(*it));
                   prodOK = true;
                   break;
                  }
//               }else if(data->getAttributeString(newUnit.getName(), DataAcc::production_state) == "producer_occupied"){
//                   // we need to register the producer and check for free capacity
//                   if(!it->isOccupied()){
//                       it->incOccupiedBy();
//                       newUnit.setOccupy(&(*it));
//                       prodOK = true;
//                       break;
//                   }
//               }
//                 else{assert(false);}
               }
        }
       }

       // if no producer in finished queue, search in building queue
       // TODO maybe only check for an empty building list, might be faster
       if(!prodExists){
           for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            for (std::vector<std::string>::iterator vec_prod = prod.begin(); vec_prod != prod.end();
                          vec_prod++) {
               if(*vec_prod == it->getName()){
                   prodExists = true;
               }
           }
           }
           // TODO use value to indicate stopping of simulation
           if(!prodExists)
               return -2;
             }

       if(prodOK){
           // add event for JSON
           addEvent("build-start", newUnit.getName(), prodName);

           // manage lists
           building.push_back(newUnit);
           std::vector<std::string> prod = data->getAttributeVector(newUnit.getName(), DataAcc::producer);
           for(std::vector<std::string>::iterator it = prod.begin(); it != prod.end(); it++) {
               if (*it == "SCV") {
                   occupyingworker++;
               }
            }
           future.remove(newUnit);

           // manage materials
           minerals -= needMin;
           vespene -= needVesp;
           supply_used += needSupply;
           return 1;
       }
       return 0;
}
