#ifndef PROTOSS_H
#define PROTOSS_H

#include "../Race.h"

class Protoss: public Race{
private:
    int energy;
    int chrono_boost;
    Unit *boosted;
public:
    Protoss(std::vector<std::string> buildorder);

    void updateResources();
    void advanceBuildingProcess();
    void specialAbility();
    int startBuildingProcess();

};
#endif
