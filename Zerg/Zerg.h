#ifndef ZERG_H
#define ZERG_H

#include "../Race.h"
#include <iostream>

class Zerg: public Race{
private:
    static int larva_num;
    static int inject_larva_num;
    static bool larva_producing;
    static int count;
  static bool base_occ;
  static bool inject;
    std::list<Unit> Queenlist;
    std::list<Unit> Zerglingpair;

//  Unit *injection;

public:
    Zerg(std::vector<std::string> buildorder);
    void updateResources();
    void advanceBuildingProcess();
    int specialAbility();
    int startBuildingProcess();
    void larvaSelfGeneration();
};

#endif
