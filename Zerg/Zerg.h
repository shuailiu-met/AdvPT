#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

#include "Race.h"
#include <iostream>

Class Zerg: public Race{
private:
public:
    Zerg(std::vector<std::string> buildorder) : Race{buildorder}{}
    int workersOnVespene();
    void Initializer();
    void harvestupdate();
}

//it will return the num of worker on gas
int Zerg::workersOnVespene()
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
    return i;
}


//I think this function should be called in the constructor,when we create the race object
void Zerg::Initializer()
{
    Units u("Hatchery",0);
    finished.push_back(u);
    Units o("Overload",0);
    finished.push_back(o);
    Units worker[12]("Drone",0);
    for(int i = 0;i<12;i++)
    {
        finished.push_back(worker[i]);
    }
}

//I think this function can be merged into the advanceOneTimeStep and this function could be put in the race class
void Zerg::harvestupdate()
{
    int worker_vespene = workersOnVespene();
    int worker_mineral = workers - worker_vespene;
    mineral_harvesting = data->getParameter("MINERAL_HARVESTING");
    vespene_harvesting = data->getParameter("VESPENE_HARVESTING");
    double add_mineral = mineral_harvesting*worker_mineral;//here the data type need to be considered.
    double add_vespene = vespene_harvesting*worker_vespene;
    minerals += add_mineral;
    vespene += add_vespene;
}


#endif // ZERG_H_INCLUDED
