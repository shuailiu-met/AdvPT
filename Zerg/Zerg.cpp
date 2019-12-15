#include "Zerg.h"
#include <iostream>
#include <string>

Zerg::Zerg(std::vector<std::string> buildorder) : Race{buildorder}{

    // set race specific attributes
    larva_num = data->getParameter("LARVA_START",false);
    larva_max = data->getParameter("MAX_LARVA_PER_BUILDING".false);
    larva_duration = data->getParameter("LARVA_DURATION",true);
    inject_larva_num = 0;
    inject_per = data->getParameter("INJECTLARVAE_AMOUNT",false);
    inject_max = data->getParameter("MAX_INJECTLARVAE_PER_BUILDING",false);
    inject_duration = data->getParameter("INJECTLARVAE_DURATION",true);
    larva_producing = 0;
    inject_cost = data->getParameter("INJECTLARVAE_ENERGY_COST",true)
    // build the starting unit
    //here don't use the getUnit() because here the building time should be set to 0
    Units u("Hatchery",0,1,false);
    finished.push_back(u);
    Units o("Overload",0,1,true);
    finished.push_back(o);
    Units worker[12]("Drone",0,1,true);
    for(int i = 0;i<12;i++)
    {
        finished.push_back(worker[i]);
    }
}

int Zerg::larva_total()
{
    return larva_num+inject_num;
}

//get the num of worker on gas,then distribute worker
void Zerg::distributeWorkers()
{
    std::string target = "Extractor";
    auto it = finished.begin();
    int i = 0;
    while(it!=finished.end())
    {
    it = find_if(it,finished.end(),[&target](const Unit &u){return (u.getName() == target);});
    if(it!=finished.end())
    {
        i++;
        it++;
    }
    }
    worker_vespene = i*3;
    worker_minerals = workers - worker_vespene;
}

//I think this function can be merged into the advanceOneTimeStep and this function could be put in the race class
void Zerg::updateResources()
{
    mineral_harvesting = data->getParameter("MINERAL_HARVESTING", true);
    vespene_harvesting = data->getParameter("VESPENE_HARVESTING", true);
    int add_mineral = mineral_harvesting*worker_mineral;//here the data type need to be considered.
    int add_vespene = vespene_harvesting*worker_vespene;
    minerals += add_mineral;
    vespene += add_vespene;
}

void Zerg::larvaSelfGeneration()
{
    if(larva_num + larva_producing + inject_larva_num < larva_max)
    {
        Unit larva("Larva",larva_duration,1,true);
        building.push_back(larva);
        larva_producing++;
    }
}

void Zerg::advanceBuildingProcess(){
    // walk over all buildings and update build time
    std::vector<Unit> finishedTemp;
    for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            it->updateTime(1 * FIXEDPOINT_FACTOR);
    // don't remove element while iterating over list
        if(it->isFinished()){
            if(it->getName()=="Larva")
            {
                larva_num++;
                larva_producing--;
                building.remove(*it);
            }
            else if(it->getName()=="injection")
            {
                inject_larva_num += inject_per;
                building.remove(*it);
            }
            else{
            if(it->getName()=="Queen")
            {
                Queenlist.push_back(*it);
            }
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
    }

    // if finished elements -> move
    for(std::vector<Unit>::iterator it = finishedTemp.begin(); it != finishedTemp.end(); it++) {
        building.remove(*it);
        finished.push_back(*it);
        std::vector<int> i = {it->getId()};
        addEvent("build-end", it->getName(), it->getBuildBy(), "", &i);
    }
}

//TODO: consider consumption at end/start/ occupy
int Zerg::startBuildingProcess()
{

}

//Reminder:this should be checked every time step,injection
void Zerg::specialAbility()
{
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    for(std::list<Unit>::iterator it = Queenlist.begin(); it != Queenlist.end(); it++)
    {
        if((it->currentEnergy() >= inject_cost)&&(inject_larva_num < inject_max))
        {
            Unit in("injection",inject_duration,1,true);
            building.push_back(in);
            setEnergy(-energy_regen);
        }

        if(it->currentEnergy() + energy_regen <= it->maxEnergy())
        {
            setEnergy(energy_regen);
        }
    }
}
