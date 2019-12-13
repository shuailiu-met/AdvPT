#ifndef UNIT_H
#define UNIT_H

#include <string>

/************************ Class Unit ***************************
 * A class representing a Building or a Unit with its current
 * parameters
 *
 * TODO think about what else we need in here
 ***************************************************************/

class Unit{
private:
    // time left to build this Unit
    int time_left;
    // name of the Unit
    std::string name;
public:
    // contructor
    Unit(std::string n, int time) : time_left(time){
        name = n;
    }

    // getter for the attributes
    std::string getName() const{return name;}
    bool isFinished(){return (time_left == 0);}
    int getTimeLeft(){return time_left;}

    // subtract the given time from time left
    int updateTime(int remove){
        time_left -= remove;
        if(time_left < 0) time_left = 0;
        return time_left;
    }
};

#endif
