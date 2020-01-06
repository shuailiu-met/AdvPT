#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

#include "../Race.h"
#include <iostream>

class Zerg: public Race{
protected:
    int mineral_harvesting;
    int vespene_harvesting;
    int larva_num;
    int larva_max;
    int larva_duration;
    int inject_larva_num;
    int inject_max;
    int inject_per;
    int inject_duration;
    int energy;
    int inject;
    int larva_producing;
    int inject_cost;
    std::list<Unit> Queenlist;

public:
    Zerg(std::vector<std::string> buildorder);
    int larva_total();
    void updateResources();
    void advanceBuildingProcess();
    int specialAbility();
    int startBuildingProcess();
    //void distributeWorkers();
    void larvaSelfGeneration();
};

#endif // ZERG_H_INCLUDED
