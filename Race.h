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

    // resources the race currently has
    int workers;
    int minerals;
    int vespene;
    int supply;
    int supply_used;

    // worker distribution
    int worker_minerals;
    int worker_vespene;

    // private helper functions for advanceTimeStep
    // - update the current resources depending on the worker assignment
    virtual void updateResources() = 0;
    // - advance the building process, returns 1 if there's still sth to do
    virtual void advanceBuildingProcess() = 0;
    // - activates and manages the special ability - race dependent
    virtual void specialAbility() = 0;
    // - start new building process if possible, returns 1 if there are buildings left
    virtual int startBuildingProcess() = 0;
    // - distribute workers depending on the strategy
    void distributeWorkers();
    // - output current JSON
    void outputJSON();

    // Event vector for JSON
    std::vector<struct JSON> json;
    std::vector<struct Event> events;
    // add Event to JSON
    // Supported Types: "build-start", "build-end", "special"
    // for parameter see examples on https://www10.cs.fau.de/advptSC2/
    void addEvent(std::string type, std::string name, std::string i1, std::string i2 = "", std::vector<int> *ids = nullptr);

    void addEventsToJSON(bool init = false);

public:
    // standard constructor
    // gets a vector of buildings to be build in that order
    Race(std::vector<std::string> buildings);
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

};

#endif
