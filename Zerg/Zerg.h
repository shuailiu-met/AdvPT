#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

#include "Race.h"
#include <iostream>

Class Zerg: public Race{
private:
    int energy;
    int larva_injection_time;
public:
    Zerg(std::vector<std::string> buildorder) : Race{buildorder}{}
    void distributeWorkers();
    void Initializer();
    void updateResources();
    void advanceBuildingProcess();
    void specialAbility();
    int startBuildingProcess();
}

#endif // ZERG_H_INCLUDED
