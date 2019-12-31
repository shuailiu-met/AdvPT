#include "Zerg.h"
#include <iostream>
#include <string>
#include <algorithm>

Zerg::Zerg(std::vector<std::string> buildorder){
    mineral_harvesting = data->getParameter("MINERAL_HARVESTING", true);
    vespene_harvesting = data->getParameter("VESPENE_HARVESTING", true);

    // set race specific attributes
    larva_num = data->getParameter("LARVA_START",false);
    larva_max = data->getParameter("MAX_LARVA_PER_BUILDING",false);
    larva_duration = data->getParameter("LARVA_DURATION",true);
    inject_larva_num = 0;
    inject_per = data->getParameter("INJECTLARVAE_AMOUNT",false);
    inject_max = data->getParameter("MAX_INJECTLARVAE_PER_BUILDING",false);
    inject_duration = data->getParameter("INJECTLARVAE_DURATION",true);
    larva_producing = 0;
    inject_cost = data->getParameter("INJECTLARVAE_ENERGY_COST",true);
    // build the starting unit
    //here don't use the getUnit() because here the building time should be set to 0
    Unit u("Hatchery",0,1,false);
    finished.push_back(u);
    Unit o("Overload",0,1,true);
    finished.push_back(o);
    //Unit worker[12]("Drone",0,1,true);
    for(int i = 0;i<12;i++)
    {
       finished.push_back(data->getUnit("Drone"));
       workers++;
    }
    supply = 14;
    supply_used = 12;

    for(std::vector<std::string>::iterator it = buildorder.begin(); it != buildorder.end(); it++) {
        Unit u = data->getUnit(*it);
        future.push_back(u);
    }
    worker_minerals = workers;
}

int Zerg::larva_total()
{
    return larva_num+inject_larva_num;
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
    int add_mineral = mineral_harvesting*worker_minerals;//here the data type need to be considered.
    int add_vespene = vespene_harvesting*worker_vespene;
    minerals += add_mineral;
    vespene += add_vespene;
    larvaSelfGeneration();
}

//TODO: need to be done each time step for Zerg
void Zerg::larvaSelfGeneration()
{
    if(larva_num + larva_producing + inject_larva_num < larva_max)
    {
        Unit larva("Larva",larva_duration,1,true);
        building.push_back(larva);
        larva_producing++;
    }
}


//TODO:think about the double Zerg
void Zerg::advanceBuildingProcess(){
    // walk over all buildings and update build time
    std::vector<Unit> finishedTemp;
    for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            it->updateTime(1 * FIXEDPOINT_FACTOR);
    // don't remove element while iterating over list
        if(it->isFinished()){
            std::string Building_state = data->getAttributeString(it->getName(),DataAcc::production_state);

            if(Building_state == "producer_consumed_at_end")
            {
                std::vector<std::string> prod = data->getAttributeVector(it->getName(),DataAcc::producer);
                for(auto it1 = prod.begin();it1!=prod.end();it1++)
                {
                    std::string target = *it1;
                    auto it2 = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName()==target);});
                    if(it2!=finished.end())
                    {
                        finished.remove(*it2);
                    }
                }
            }

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
            supply += data->getAttributeValue(it->getName(), DataAcc::supply, false);
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
        //std::vector<int> i = {it->getId()};
         addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
    }
}

//TODO: consider consumption at end/start/ occupy
int Zerg::startBuildingProcess()
{
    if(future.size()==0)
    {
        return 0;
    }

    Unit newUnit = future.front();
    if (data->getAttributeString(newUnit.getName(),DataAcc::race)!="Zer")
    {
        return -2;
    }

    //check resource

        int scost = data->getAttributeValue(newUnit.getName(),DataAcc::supply_cost,false);
        int vcost = data->getAttributeValue(newUnit.getName(),DataAcc::vespene,true);
        int mcost = data->getAttributeValue(newUnit.getName(),DataAcc::minerals,true);
        if((scost>supply-supply_used)||(vcost>vespene)||(mcost>minerals))
        {
            if((vcost>vespene)||(mcost>minerals))
            {
                return -2;
            }

            if((scost>supply-supply_used)&&(building.size()==0))
            {
                return -2;
            }
            return 0;
        }

        //check deps
            std::string deps = data->getAttributeString(newUnit.getName(),DataAcc::dependencies);
            bool found = false;
            auto it = find_if(finished.begin(),finished.end(),[&deps](const Unit &u){return (u.getName() == deps);});
            if(it!=finished.end())
            {
                found = true;
            }
            // find if it's building

            if(found == false)
            {
                auto it2 = find_if(building.begin(),building.end(),[&deps](const Unit &u){return (u.getName()==deps);});
                if(it2!=building.end())
                {
                    return 0;
                }
                else
                {
                    return -2;
                }
            }


            bool prod_exist = false;
            bool prod_ok = false;
            std::string cur_producer = "";
            std::string prodName = "";
            std::vector<std::string> prod = data->getAttributeVector(newUnit.getName(),DataAcc::producer);
            for(auto it3 = prod.begin();it3!=prod.end();it3++)
            {
                std::string target = *it3;
                auto it4 = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName()==target);});
                if(it4!=finished.end())
                {
                    prod_exist = true;
                    cur_producer = target;
                    prodName = it->getName() + "_" + cur_producer;
                    newUnit.setBuildBy(prodName);
                    break;
                }
            }

            //check if producer is in building progress
            if (prod_exist==false)
            {
                for (auto it5 = prod.begin();it5!=prod.end();it5++)
                {
                    std::string target = *it5;
                    auto it6 = find_if(building.begin(),building.end(),[&target](const Unit &u){return (u.getName()==target);});
                    if(it6!=building.end())
                    {
                        return 0;
                    }
                }
                return -2;
            }

            //consider sacrifice and occupy
            if(prod_exist==true)
            {
            std::string Building_state = data->getAttributeString(newUnit.getName(),DataAcc::production_state);

            if(Building_state == "producer_consumed_at_start")
            {
                if(data->getAttributeString(newUnit.getName(),DataAcc::structure)=="false")
                {
                    if(larva_num!=0)
                    {
                        larva_num --;
                        prod_ok = true;
                    }
                    else if(inject_larva_num!=0)
                    {
                        inject_larva_num --;
                        prod_ok = true;
                    }
                    else
                    {
                        return -2;
                    }
                }

                if(data->getAttributeString(newUnit.getName(),DataAcc::structure)=="true")
                {
                    auto it7 = find_if(finished.begin(),finished.end(),[&cur_producer](const Unit &u){return (u.getName()==cur_producer);});
                    finished.remove(*it7);
                    if(cur_producer=="Drone")
                    {
                        worker_minerals--;
                        workers = worker_minerals+worker_vespene;
                    }
                    prod_ok = true;
                }
                //"Unit or Building"
                //check producer here and eliminate the first producer we found.
                //worker number,worker on minerals, supply
            }
            //if(Building_state == "producer_consumed_at_end")//maybe better in advanced step

            if(Building_state == "producer_occupied")
            {
                auto it8 = find_if(finished.begin(),finished.end(),[&cur_producer](const Unit &u){return (u.getName()==cur_producer);});
                if(it8->isOccupied()!=1)
                {
                    it8->incOccupiedBy();
                    newUnit.setOccupy(&(*it8));
                    prod_ok = true;
                }
                //check occupy here
            }

            if(prod_ok==true)
            {
                addEvent("build-start", newUnit.getName(), prodName);
                building.push_back(newUnit);
                finished.remove(newUnit);
                minerals -= mcost;
                vespene -= vcost;
                supply_used += scost;
                return 1;
            }
            }

            return 0;
    //check deps,producer,producer elimination,double produced
}

//Reminder:this should be checked every time step,injection
int Zerg::specialAbility()
{
    //TODO ,return 1,0 is now only considering 1 queen.
    //TODO , not sure about the id and JSON
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    for(std::list<Unit>::iterator it = Queenlist.begin(); it != Queenlist.end(); it++)
    {
        if((it->currentEnergy() >= inject_cost)&&(inject_larva_num < inject_max))
        {
            std::string id = it->getName() + "_" + std::to_string(it->getId());
            Unit in("injection",inject_duration,1,true);
            building.push_back(in);
            it->setEnergy(-inject_cost);
            addEvent("special","injection",id,id);
            return 1;
        }

        if(it->currentEnergy() + energy_regen <= it->maxEnergy())
        {
            it->setEnergy(energy_regen);
        }
    }
    return 0;
}
