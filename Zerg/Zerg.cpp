#include "Zerg.h"
#include <iostream>
#include <string>
#include <algorithm>


//problems now : zergling(actually queen problem) , invalid output, queen, why only 2
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
    for(int i = 0; i < data->getParameter("BASIS_START"); i++){
        Unit u = data->getUnit("Hatchery");
        finished.push_back(u);
    }
    for(int i = 0; i < data->getParameter("WORKERS_START"); i++){
        finished.push_back(data->getUnit("Drone"));
        workers++;
    }
    finished.push_back(data->getUnit("Overlord"));
    supply = 14;
    supply_used = 12;

    for(std::vector<std::string>::iterator it = buildorder.begin(); it != buildorder.end(); it++) {
        Unit u = data->getUnit(*it);
        if(*it == "Zergling")
        {
            Unit p = data->getUnit("Zergling");
            Zerglingpair.push_back(p);
        }
        /*auto it2 = it++;
        if((*it == "Zergling")&&(*it2 == "Zergling"))
        {
            Unit u = data->getUnit(*it);
            buildorder.erase(it2);
        }*/
        future.push_back(u);
    }
    worker_minerals = workers;

}

int Zerg::larva_total()
{
    return larva_num+inject_larva_num;
}




//I think this function can be merged into the advanceOneTimeStep and this function could be put in the race class
void Zerg::updateResources()
{
    int add_mineral = mineral_harvesting*worker_minerals;//here the data type need to be considered.
    int add_vespene = vespene_harvesting*worker_vespene;
    minerals += add_mineral;
    vespene += add_vespene;
    //std::cout << minerals << std::endl;
    //std::cout << vespene << std::endl;
    larvaSelfGeneration();
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    for (auto it = Queenlist.begin();it!=Queenlist.end();it++)
    {
    if(it->currentEnergy() + energy_regen <= it->maxEnergy())
    {
        it->setEnergy(energy_regen);
    }
    }
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

            else if(it->getName()=="Queen")
            {
                Queenlist.push_back(*it);
                finishedTemp.push_back(*it);
            }
            else
            {
            finishedTemp.push_back(*it);
            // add supply if building provides it
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
        if(it->getName() == "Drone")
            workers++;
        if(it->getName() == "Extractor")
            vespene_buildings += 1;
        if(it->getName() == "Hatchery")
            supply += 6;
        if(it->getName() == "Overlord")
            supply += 8;
        //std::vector<int> i = {it->getId()};
        //Zergling became 2 when finish building
        if(it->getName() == "Zergling")
        {
            /*Unit u = data->getUnit("Zergling");
            finished.push_back(u);*/
            addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
            auto pairit = Zerglingpair.begin();
            finished.push_back(*pairit);
            addEvent("build-end", pairit->getName(), it->getBuildBy(), "", pairit->getId());
            Zerglingpair.remove(*pairit);
        }
        else
        {
         addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
        }
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
       //std::cout << newUnit.getId() << std::endl;
    if(data->getAttributeString(newUnit.getName(),DataAcc::race)!="Zer")
    {
        return -2;
    }

    //check resource
                //std::cout << newUnit.getName() << std::endl;
        int scost = data->getAttributeValue(newUnit.getName(),DataAcc::supply_cost,false);
        int vcost = data->getAttributeValue(newUnit.getName(),DataAcc::vespene,true);
        int mcost = data->getAttributeValue(newUnit.getName(),DataAcc::minerals,true);
        if(newUnit.getName()=="Zergling")
        {
            scost = 1;
        }
        //std::cout << larva_num << std::endl;
        //std::cout << supply-supply_used << std::endl;
        //std::cout << mcost << std::endl;
        if((supply-supply_used-scost<0)||(vcost>vespene)||(mcost>minerals))
        {
                            //std::cout << newUnit.getName()<< std::endl;
            if((vcost>vespene)&&(building.size()==0)&&(vespene_buildings==0))
            {
                return -2;
            }

            if((supply-supply_used-scost<0)&&(building.size()==0))
            {
                return -2;
            }
            return 0;
        }

        //check deps
            std::vector<std::string> deps = data->getAttributeVector(newUnit.getName(),DataAcc::dependencies);
            bool found = false;
            if(deps.size()!=0)
            {
            for (auto finddeps = deps.begin();finddeps!=deps.end();finddeps++)
            {
            std::string target = *finddeps;
            auto it = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName() == target);});
            if(it!=finished.end())
            {
                found = true;
                break;
            }
            }
            // find if it's building

            if(found == false)
            {
                bool future = false;
                for (auto findfuture = deps.begin();findfuture!=deps.end();findfuture++){
                std::string target = *findfuture;
                auto it2 = find_if(building.begin(),building.end(),[&target](const Unit &u){return (u.getName()==target);});
                if(it2!=building.end())
                {
                    future = true;
                    return 0;
                }
                }
                if(future == false)
                {
                    return -2;
                }
            }
            }

            else
            {
                found = true;
            }


            bool prod_exist = false;
            bool prod_ok = false;
            std::string cur_producer = "";
            std::string prodName = "";
            std::vector<std::string> prod = data->getAttributeVector(newUnit.getName(),DataAcc::producer);
            auto findlarva = prod.begin();
            if((*findlarva == "Larva")&&(larva_num+inject_larva_num!=0))
            {
                prod_exist = true;
                cur_producer = "Larva";
                prodName = "Larva";
                newUnit.setBuildBy(prodName);
            }
            else if((*findlarva == "Larva")&&(larva_num+inject_larva_num==0))
            {
                return 0;
            }
            else
            {
            for(auto it3 = prod.begin();it3!=prod.end();it3++)
            {
                std::string target = *it3;
                auto it4 = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName()==target);});
                if(it4!=finished.end())
                {
                    prod_exist = true;
                    std::string s = std::to_string(it4->getId());
                    prodName = it4->getName() + "_" + s;
                    newUnit.setBuildBy(prodName);
                    //prod_exist = true;
                    cur_producer = target;
                   // prodName = it4->getName() + "_" + cur_producer;
                    //newUnit.setBuildBy(prodName);
                    break;
                }
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
                if(data->getAttributeString(newUnit.getName(),DataAcc::structure)=="False")
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
                }

                if(data->getAttributeString(newUnit.getName(),DataAcc::structure)=="True")
                {
                    auto it7 = find_if(finished.begin(),finished.end(),[&cur_producer](const Unit &u){return (u.getName()==cur_producer);});
                    finished.remove(*it7);
                    if(cur_producer=="Drone")
                    {
                        worker_minerals--;
                        workers = worker_minerals+worker_vespene;
                        supply_used--;
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
            //std::cout << newUnit.getName() << std::endl;
            if(Building_state == "producer_consumed_at_end")
            {
                prod_ok = true;
            }

            if(prod_ok==true)
            {
                //std::cout << newUnit.getName() <<std::endl;
                addEvent("build-start", newUnit.getName(), prodName);
                building.push_back(newUnit);
                future.remove(newUnit);
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
    }
    return 0;
}
