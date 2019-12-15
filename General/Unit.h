#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <assert.h>
#include <iostream>

/************************ Class Unit ***************************
 * A class representing a Building or a Unit with its current
 * parameters
 *
 * TODO think about what else we need in here
 ***************************************************************/

// global incrementing id variable to compare Unit Objects
// TODO maybe not the best solutions for this problem
// -> global data is always bad
extern int global_id;

class Unit{
private:
    // unique id for comparison operators
    int id;
    // time left to build this Unit
    int time_left;
    // name of the Unit
    std::string name;
    // is unit occupy building sth at the moment
    int occupied_by_limit;
    int occupied_by_cur;
    // Object which is occupied by the building process of this Unit
    Unit *occupy;
    // is a unit or building
    bool is_unit;

    // build by this building -> used for JSON
    std::string build_by;

public:
    // contructor
    Unit(std::string n, int time, int occ_limit, bool unit) : time_left(time), occupied_by_limit(occ_limit), is_unit(unit){
        // set id
        id = global_id;
        id = 0;
        global_id++;

        // set values
        name = n;
        occupied_by_cur = 0;
        occupy = nullptr;
    }

    // getter and setter for the attributes
    std::string getName() const{return name;}
    int getId(){return id;}
    bool isFinished(){return (time_left == 0);}
    int getTimeLeft(){return time_left;}
    std::string getBuildBy(){return build_by;}
    void setBuildBy(std::string bb){build_by = bb;}

    // check the occupation status of this building
    bool isOccupied(){return (occupied_by_cur == occupied_by_limit);}
    bool incOccupiedBy(){
        if(occupied_by_limit > occupied_by_cur){
            occupied_by_cur += 1;
            return true;
        }else{
            return false;
        }
    }
    void decOccupyBy(){
        if(occupied_by_cur <= 0)
            assert(false);
        occupied_by_cur -= 1;
    }

    // set a building we occupy during the building process
    void setOccupy(Unit *u){
        assert((u == nullptr && occupy != nullptr) || (u != nullptr && occupy == nullptr));
        occupy = u;
    }
    Unit* getOccupy(){return occupy;}

    // subtract the given time from time left
    int updateTime(int remove){
        time_left -= remove;
        if(time_left < 0) time_left = 0;
        return time_left;
    }

    // comparison operators, these are needed to remove objects from a list
    bool operator == (const Unit& s) const { return name == s.name && id == s.id; }
    bool operator != (const Unit& s) const { return !operator==(s); }
};

#endif
