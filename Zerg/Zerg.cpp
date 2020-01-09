#include "Zerg.h"
#include <iostream>
#include <string>
#include <algorithm>

int Zerg::larva_num = 3;
int Zerg::inject_larva_num = 0;
bool Zerg::larva_producing = false;
int Zerg::count = 0;

Zerg::Zerg(std::vector<std::string> buildorder){
    for(int i = 0; i < data->getParameter("BASIS_START"); i++){
        Unit u = data->getUnit("Hatchery");
        finished.push_back(u);
        supply = data->getAttributeValue(u.getName(),DataAcc::supply_provided,false);
    }
    for(int i = 0; i < data->getParameter("WORKERS_START"); i++){
        finished.push_back(data->getUnit("Drone"));
        workers++;
        supply_used +=data->getAttributeValue("Drone", DataAcc::supply_cost, false);
    }
    finished.push_back(data->getUnit("Overlord"));
    supply += data->getAttributeValue("Overlord",DataAcc::supply_provided,false);

    for(std::vector<std::string>::iterator it = buildorder.begin(); it != buildorder.end(); it++) {
        Unit u = data->getUnit(*it);
        if(u.getName() == "Zergling")
        {
            Unit p = data->getUnit("Zergling");
            Zerglingpair.push_back(p);
        }
        future.push_back(u);
    }
    worker_minerals = workers;
   // std::cout << data->getAttributeString("Injection",DataAcc::race);

}


//I think this function can be merged into the advanceOneTimeStep and this function could be put in the race class
void Zerg::updateResources()
{
    std::vector<Unit> LarvaeTemp;
    /*std::cout << larva_num << std::endl;
    std::cout << larva_producing << std::endl << std::endl;

        for(auto it = building.begin();it!=building.end();it++)
        {
            std::cout << it->getName() << " " << std::endl;
        }*/
    //std::cout << larva_num << std::endl;
    if(worker_minerals > 0){
        minerals += worker_minerals * data->getParameter("MINERAL_HARVESTING", true);
    }
    if(worker_vespene > 0){
        vespene += worker_vespene * data->getParameter("VESPENE_HARVESTING", true);
    }
    //larvaSelfGeneration();
    int energy_regen = data->getParameter("ENERGY_REGEN_RATE", true);
    for (std::list<Unit>::iterator it = Queenlist.begin();it!=Queenlist.end();it++)
    {
    if(it->currentEnergy() + energy_regen <= it->maxEnergy())
    {
        it->setEnergy(energy_regen);
    }
    }

    if((larva_num < 3)&&(larva_producing == false))
    {
            Unit l = data->getUnit("Larva");
            building.push_back(l);
            LarvaeTemp.push_back(l);
            //std::cout << l.unitornot() << std::endl;
            larva_producing=true;
            //larva_producing++;
    }
}

/*void Zerg::larvaSelfGeneration()
{
    if(larva_num < data->getParameter("MAX_LARVA_PER_BUILDING",false))
    {
        Unit larva("Larva",data->getParameter("LARVA_DURATION",true),1,true);
        building.push_back(larva);
        larva_producing++;
    }
}*/


void Zerg::advanceBuildingProcess(){
    //TODO, Event ids
    std::vector<Unit> finishedTemp;
    //std::vector<Unit> LarvaeTemp;
    for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
            it->updateTime(1 * FIXEDPOINT_FACTOR);
    // don't remove element while iterating over list
        if(it->isFinished()){
            if(it->getName()=="Injection")
            {
                finishedTemp.push_back(*it);
            }
            else{
             /*if(it->getName()=="injection")
            {
                //std::string id = it->getName() + "_" + std::to_string(it->getId());
                inject_larva_num += data->getParameter("INJECTLARVAE_AMOUNT",false);
                building.remove(*it);
            }*/
            /*else if(it->getName()=="Larva")
            {
                building.remove(*it);
                LarvaeTemp.push_back(*it);
                larva_num++;
                larva_producing--;
                addEvent("build-end", it->getName(), it->getName(), "", 0);
            }*/

            std::string Building_state = data->getAttributeString(it->getName(),DataAcc::production_state);

            if(Building_state == "producer_consumed_at_end")
            {
                std::vector<std::string> prod = data->getAttributeVector(it->getName(),DataAcc::producer);
                for(std::vector<std::string>::iterator it1 = prod.begin();it1!=prod.end();it1++)
                {
                    for(std::list<Unit>::iterator it2 = finished.begin();it2!=finished.end();it2++)
                    {
                    if(it2->getName()==*it1)
                    {
                        finished.remove(*it2);
                        finishedTemp.push_back(*it);
                        break;
                    }
                    }
                }
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
        }


    // if finished elements -> move

    for(std::vector<Unit>::iterator it = finishedTemp.begin(); it != finishedTemp.end(); it++) {
        building.remove(*it);
        if(it->getName()=="Larva")
        {
            larva_num++;
            larva_producing = false;
        }
        else if((it->getName()=="Injection"))
        {
            inject_larva_num +=3;
        }
        else
        {
        finished.push_back(*it);
        if(it->getName() == "Drone")
            workers++;
        if(it->getName() == "Extractor")
            vespene_buildings ++;
        if(it->getName() == "Hatchery")
            supply += 6;
        if(it->getName() == "Overlord")
            supply += 8;
        if(it->getName() == "Queen")
        {
            Queenlist.push_back(*it);
        }
        if(it->getName() == "Zergling")
        {
            addEvent("build-end", it->getName(), it->getBuildBy(), "", it->getId());
            std::list<Unit>::iterator pairit = Zerglingpair.begin();
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
}

//TODO: consider consumption at end/start/ occupy
int Zerg::startBuildingProcess()
{
    if(future.size()==0)
    {
        return 0;
    }

    Unit newUnit = future.front();
    if(data->getAttributeString(newUnit.getName(),DataAcc::race)!="Zerg")
    {
        return -2;
    }

    //check resource
        int scost = data->getAttributeValue(newUnit.getName(),DataAcc::supply_cost,false);
        int vcost = data->getAttributeValue(newUnit.getName(),DataAcc::vespene,true);
        int mcost = data->getAttributeValue(newUnit.getName(),DataAcc::minerals,true);
        if(newUnit.getName()=="Zergling")
        {
            scost = 1;
        }
        if((supply-supply_used-scost<0)||(vcost>vespene)||(mcost>minerals))
        {
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
            for (std::vector<std::string>::iterator finddeps = deps.begin();finddeps!=deps.end();finddeps++)
            {
            /*std::string target = *finddeps;
            auto it = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName() == target);});
            if(it!=finished.end())
            {
                found = true;
                break;
            }*/
            for(std::list<Unit>::iterator it =finished.begin();it!=finished.end();it++)
            {
                if(it->getName()==*finddeps)
                {
                    found = true;
                    break;
                }
            }
            }
            // find if it's building

            if(found == false)
            {
                bool future = false;
                for (std::vector<std::string>::iterator findfuture = deps.begin();findfuture!=deps.end();findfuture++){
                /*std::string target = *findfuture;
                auto it2 = find_if(building.begin(),building.end(),[&target](const Unit &u){return (u.getName()==target);});
                if(it2!=building.end())
                {
                    future = true;
                    return 0;
                }*/
                for(std::list<Unit>::iterator it = building.begin();it!=building.end();it++)
                {
                    if(it->getName()==*findfuture)
                    {
                        future = true;
                        return 0;
                        break;
                    }
                }
                }

                if(future == false)
                {
                    return -2;
                }
            }
            }
            //no deps
            else
            {
                found = true;
            }


            bool prod_exist = false;
            bool prod_ok = false;
            std::string cur_producer = "";
            std::string prodName = "";
            std::vector<std::string> prod = data->getAttributeVector(newUnit.getName(),DataAcc::producer);
            std::vector<std::string>::iterator findlarva = prod.begin();
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
            for(std::vector<std::string>::iterator it3 = prod.begin();it3!=prod.end();it3++)
            {
                /*std::string target = *it3;
                auto it4 = find_if(finished.begin(),finished.end(),[&target](const Unit &u){return (u.getName()==target);});*/
                for(std::list<Unit>::iterator it4 = finished.begin();it4!=finished.end();it4++)
                {
                if(it4->getName()==*it3)
                {
                    prod_exist = true;
                    std::string s = std::to_string(it4->getId());
                    prodName = it4->getName() + "_" + s;
                    newUnit.setBuildBy(prodName);
                    //prod_exist = true;
                    cur_producer = it4->getName();
                   // prodName = it4->getName() + "_" + cur_producer;
                    //newUnit.setBuildBy(prodName);
                    break;
                }
                }
            }
            }

            //check if producer is in building progress
            if (prod_exist==false)
            {
                for (std::vector<std::string>::iterator it5 = prod.begin();it5!=prod.end();it5++)
                {
                    /*std::string target = *it5;
                    auto it6 = find_if(building.begin(),building.end(),[&target](const Unit &u){return (u.getName()==target);});*/
                    for(std::list<Unit>::iterator it6 = building.begin();it6!=building.end();it6++)
                    {
                    if(it6->getName()==*it5)
                    {
                        return 0;
                        break;
                    }
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
                        /*Unit l = data->getUnit("Larva");
                        building.push_back(l);
                        //std::cout << "larva" << std::endl;
                        larva_producing++;*/
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
                    cur_producer = data->getAttributeString(newUnit.getName(),DataAcc::producer);
                    std::list<Unit>::iterator it7 = find_if(finished.begin(),finished.end(),[&cur_producer](const Unit &u){return (u.getName()==cur_producer);});
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
                std::vector<std::string> occ_producer = data->getAttributeVector(newUnit.getName(),DataAcc::producer);
                for(std::vector<std::string>::iterator it9 = occ_producer.begin();it9!=occ_producer.end();it9++)
                {
                    for(std::list<Unit>::iterator it8 = finished.begin();it8!=finished.end();it8++)
                    {
                        if(it8->getName()==*it9)
                        {
                            if(it8->isOccupied()!=1)
                            {
                                it8->incOccupiedBy();
                                newUnit.setOccupy(&(*it8));
                                prod_ok = true;
                                break;
                            }
                        }
                    }
                /*std::list<Unit>::iterator it8 = find_if(finished.begin(),finished.end(),[&cur_producer](const Unit &u){return (u.getName()==cur_producer);});*/
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
                //std::cout << newUnit.getName() << std::endl;
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
    //TODO , event target ids
   for(std::list<Unit>::iterator it = Queenlist.begin(); it != Queenlist.end(); it++)
    {
        std::string id = it->getName() + "_" + std::to_string(it->getId());
        if((it->currentEnergy() >= data->getParameter("INJECTLARVAE_ENERGY_COST",true))&&(inject_larva_num < 19))
        {
            std::cout << "here" << std::endl;
            Unit in = data->getUnit("Injection");
            building.push_back(in);
           //larva_producing++;
//injection = new Unit("injection",data->getParameter("INJECTLARVAE_DURATION",true),1,true,0,0,0);
            //std::cout << in.getName() << std::endl;
            //building.push_back(*injection);
            it->setEnergy(-data->getParameter("INJECTLARVAE_ENERGY_COST",true));
            /*std::string hat = "Hatchery"
            auto target = find_if(finished.begin(),finished.end(),[])*/
            addEvent("special","injection_begin","",id);
            return 1;

        }
    }
    return 0;
}
