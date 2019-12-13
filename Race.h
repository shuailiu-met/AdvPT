#ifndef RACE_H
#define RACE_H

#include <list>
#include <vector>
#include <string>
#include "General/Unit.h"
#include "General/DataAcc.h"

/*********************** Class Race ****************************
 * A Stencil for one of the three races
 * Contains all shared functions and attributes
 *
 * TODO think about worker distribution
 ***************************************************************/

class Race {
private:
    // list for finished Units the race currently has
    std::list<Unit> finished;
    // list for Units currently building
    std::list<Unit> building;
    // list for units to be build
    std::list<Unit> future;

    // DataAcc object for all neccesarry parameters
    DataAcc *data;

    // remember the current simulation time
    size_t currentTime;

    // resources the race currently has
    int workers;
    int minerals;
    int vespene;
    int supply;
public:
    // standard constructor
    // gets a vector of buildings to be build in that order
    Race(std::vector<std::string> buildings);

    // is called from the simulation to advance on time step
    // 1.) Update Resources
    // 2.) Advance Building Process
    // 3.) Special Ability
    // 4.) Start new building Process
    // 5.) Distribute workers
    // TODO Can partly be implemented in this common class
    // and partly in the derived Race
    // RETURNS: 1 if process not finished
    //          0 if build process finished
    int advanceOneTimeStep();

};

#endif
