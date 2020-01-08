//
//  Terran.h
//  SC2-4
//
//  Created by 宋經緯 on 2019/12/15.
//  Copyright © 2019 宋經緯. All rights reserved.
//

#ifndef Terran_h
#define Terran_h
#include <vector>
#include "Race.h"
class Terran: public Race{
private:
    int energy;
    std::list<Unit> hasEnergy;
    int duration;
    int mule_duration;
    std::unordered_map<std::string, int> mule;
    bool found = false;
    bool releasemule = false;
public:
    Terran(std::vector<std::string> buildorder);

    void updateResources();
    void advanceBuildingProcess();
//    bool  specialAbility();
    int  startBuildingProcess();
    void distributeWorkers();
    int specialAbility();
    void addspecialability();

//    int checkdependency();

};

#endif /* Terran_h */
