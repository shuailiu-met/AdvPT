#ifndef PROTOSS_H
#define PROTOSS_H

#include "../Race.h"
#include <unordered_map>

class Protoss: public Race{
private:
    std::list<Unit> hasEnergy;
    std::unordered_map<std::string, int> chronoboost;

    int vesp_cur_build;
public:
    Protoss(DataAcc *dat, std::vector<std::string> buildorder, bool simulate = true);

    void updateResources();
    void advanceBuildingProcess();
    int specialAbility();
    int startBuildingProcess();
    void distributeWorkers();

};
#endif
