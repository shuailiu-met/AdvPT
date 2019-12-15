#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

#include "Race.h"
#include <iostream>

Class Zerg: public Race{
private:
    int larva_num;
    int larva_max;
    int larva_duration;
    int inject_num;
    int inject_max;
    int inject_per;
    int inject_duration;
    int energy;
    int inject;
    int larva_producing;
public:

    Zerg(std::vector<std::string> buildorder) : Race{buildorder}{}
    int larva_total();
    void distributeWorkers();
    void updateResources();
    void advanceBuildingProcess();
    void specialAbility();//for Queen
    int startBuildingProcess();
    void larvaSelfGeneration();
    void injection();//TODO:for the injection,connected with Queen
}

#endif // ZERG_H_INCLUDED
