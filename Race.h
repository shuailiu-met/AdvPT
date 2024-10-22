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

// Remember an Event for the JSON output
struct Event{
    std::string type;
    std::string name;
    std::string info1;
    std::string info2;
    std::vector<int> ids;
};

struct JSON{
    size_t time;
    int work_minerals;
    int work_vespene;
    int minerals;
    int vespene;
    int supply_used;
    int supply;
    std::vector<struct Event> events;
};

class Race {
protected:
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

    // will this object simulate or optimize
    // -> no JSON output for optimization
    bool simulate;
    bool buildlist_valid;

    // current build id
    uint32_t build_id;

    // resources the race currently has
    int workers;
    int minerals;
    int vespene;
    int supply;
    int supply_used;
    int vespene_buildings;
    int base_buildings;

    // worker distribution
    int worker_minerals;
    int worker_vespene;

    // private helper functions for advanceTimeStep
    // - update the current resources depending on the worker assignment
    virtual void updateResources() = 0;
    // - advance the building process, returns -2 if the simulation failed
    virtual void advanceBuildingProcess() = 0;
    // - activates and manages the special ability - race dependent
    //   returns 1 if ability was activated, otherwise 0
    virtual int specialAbility() = 0;
    // - start new building process if possible, returns 1 if a building was build
    //   returns -2 if the simulation process failed
    virtual int startBuildingProcess() = 0;
    // - distribute workers depending on the strategy
    bool distributeWorkers();
    // - output current JSON
    void outputJSON();

    // Event vector for JSON
    std::vector<struct JSON> json;
    std::vector<struct Event> events;
    // add Event to JSON
    // Supported Types: "build-start", "build-end", "special"
    // for parameter see examples on https://www10.cs.fau.de/advptSC2/
    void addEvent(std::string type, std::string name, std::string i1, std::string i2 = "", int id = -1);

    void addEventsToJSON(bool init = false, bool dist_only = false);
    void invalidateJSON();

public:
    // standard constructor
    // gets a vector of buildings to be build in that order
    Race(DataAcc *dat, bool sim = true);
    ~Race(){}

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

    // get the current simulation time
    int getSimulationTime(){return currentTime;}
    // check if the buildlist is valid
    bool buildlistValid(){return buildlist_valid;}

};

#endif
