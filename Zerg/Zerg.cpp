#include "Zerg.h"
#include <iostream>
#include <string>

//it will return the num of worker on gas
void distributeWorkers();
{
    string target = "Extractor";
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


//I think this function should be called in the constructor,when we create the race object
void Zerg::Initializer()
{
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
