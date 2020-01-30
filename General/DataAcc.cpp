#include "DataAcc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <type_traits>
#include <algorithm>
#include <cstdlib>

void DataAcc::recursiveDependencyHelper(std::vector<std::string> *bo, std::string dep, int *cur_supply, int *vespene_buildings, std::string race, bool toplevel){
    // if dependency exists - return
    if(std::count(bo->begin(), bo->end(), dep))
        return;

    // we always satisfy the main building or worker dependency
    if(dep == special[race][0])
        return;
    if(dep == special[race][1])
        return;

    // add the producer for the current dep
    std::vector<std::string> deps = this->getAttributeVector(dep, producer);
    for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
        this->recursiveDependencyHelper(bo, *it, cur_supply, vespene_buildings, race, false);
        // TODO for now always use only the first
        // not relevant in protoss, in other races there can be multiple producers
        break;
    }
    // add all dependencies for current dep
    deps = this->getAttributeVector(dep, dependencies);
    for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
        this->recursiveDependencyHelper(bo, *it, cur_supply, vespene_buildings, race, false);
    }

    // add the given dependency but not the unit itself
    if(!toplevel){
        while(*cur_supply < this->getAttributeValue(dep, supply_cost)){
            bo->push_back(special[race][3]);
            *cur_supply += this->getAttributeValue(special[race][3], supply_provided);
        }
        if(this->getAttributeValue(dep, vespene) > 0 && *vespene_buildings == 0){
            bo->push_back(special[race][2]);
            *vespene_buildings += 1;
        }

        *cur_supply += this->getAttributeValue(dep, supply);
        bo->push_back(dep);
    }
}

std::vector<std::string> DataAcc::getRandomBuildorder(std::string race, std::string target, std::string strategy, int count){
    std::vector<std::string> bo;

    // add random number of vespene buildings at the beginning!
    // mostly needed but seldom added
    // min: 0, max: 2
    //int vespene_buildings = rand() % 3;
    int vespene_buildings = 0;
    int base_buildings = 1;
    int cur_supply = this->getAttributeValue(special[race][0], supply_provided);

    // add start supply
    for(int i = 0; i < this->getParameter("WORKERS_START"); i++){
        cur_supply -= this->getAttributeValue(special[race][1], supply_cost);
    }

    // add the neccessary dependencies for our target
    this->recursiveDependencyHelper(&bo, target, (int*)&cur_supply, &vespene_buildings, race);

    // add the given amount of Units
    uint32_t to_add = 10000;
    if(strategy == "rush")
        to_add = count - bo.size();

    int target_count = 0;
    for(uint32_t i = 0; i < to_add; i ++){
        if(strategy == "push" && target_count == count){
            break;
        }
        bool added = false;
        while(!added){
            // get the random unit
            //std::string unit = ids[rand() % size + off];
            // TODO does only work for protoss, other races need to be added to special!!!
            // only add buildings that are usefull
            // -> worker, base, vespene, supply
            int random = rand() % 15;
            std::string unit;
            if(random < 4){
                unit = special[race][random];
            }else{
                unit = target;
            }

            // Not enough supply
            if((cur_supply + this->getAttributeValue(unit, supply)) < 0)
                continue;

            // check if needed dependencies exists
            std::vector<std::string> deps = this->getAttributeVector(unit, dependencies);
            bool deps_ok = true;
            for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
                // If not fount generate another random Unit
                if(!std::count(bo.begin(), bo.end(), *it)){
                    deps_ok = false;
                    break;
                }
            }
            if(!deps_ok){
                continue;
            }

            // check if producer exists
            deps = this->getAttributeVector(unit, producer);
            bool prod_ok = false;
            for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
                // If not found generate another random Unit
                // check whether it's the main building or in the list already
                if(*it == special[race][0] || std::count(bo.begin(), bo.end(), *it)){
                    prod_ok = true;
                    break;
                }
            }
            if(!prod_ok){
                continue;
            }

            if(unit == special[race][2]){
                if(vespene_buildings == base_buildings * 2){
                    continue;
                }
                vespene_buildings += 1;
            } else if(this->getAttributeValue(unit, vespene) > 0){
                if(vespene_buildings == 0){
                    continue;
                }
            }

            if(unit == special[race][0])
                base_buildings ++;

            if(unit == target)
                target_count ++;

            bo.push_back(unit);
            cur_supply += this->getAttributeValue(unit, supply);
            added = true;
        }
    }

    // TODO list should be valid based on dependencies
    // but there is still no guarantee that we can build the list
    // because of vespene (none or more than 2 buildings) or timeout

    return bo;
}

int DataAcc::parseCsvLine(std::string line, bool initLine){
    std::string id;
    std::string cell;
    std::vector<std::string> attributes;

    // theres a trailing '\r' in the line -> delete
    line.erase(line.size() - 1);
    std::stringstream lineStream(line);

    int count = 0;
    while(std::getline(lineStream, cell, ',')){
        // the first line has all the attribute names
        // -> will be ids for attribute access
        if(initLine){
            if(count != 0){
                indices.push_back(cell);
            }
        // all the other lines are attributes
        }else{
            // the first element is the name of the unit
            // -> will be the id for unit access
            if(count == 0){
                id = cell;
                ids.push_back(cell);
            }else{
                attributes.push_back(cell);
            }
        }
        count++;
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty()){
        // if this happens the first line in the csv file is broken
        if(initLine){
            return -1;
        }else{
            // If there was a trailing comma then add an empty element.
            attributes.push_back("");
        }
    }
    if(!initLine)
        data[id] = attributes;
    return 0;
}

int DataAcc::parseConfigLine(std::string line){
    std::string id;
    std::string cell;

    // theres a trailing '\r' in the line -> delete
    //line.erase(line.size() - 1);
    std::stringstream lineStream(line);

    int count = 0;
    while(std::getline(lineStream, cell, ',')){
        // the first element is the name of the unit
        // -> will be the id for unit access
        if(count == 0){
            id = cell;
        }else{
            settings[id] = cell;
        }
        count++;
    }

    // This checks for a trailing comma with no data after it.
    if ((!lineStream && cell.empty()) || count != 2){
        // If there was a trailing comma then no value is given to setting
        // -> error
        return -1;
    }
    return 0;
}


DataAcc::DataAcc(std::string unitdb, std::string config){
    cur_unit_id = 0;
    special_id = 0;

    std::fstream fs;

    // open and parse unit database file
    fs.open(unitdb, std::fstream::in);
    int count = 0;
    std::string line;
    while(std::getline(fs, line)){
        // ignore comment lines
        if(line[0] == '#'){
            count+=1;
            continue;
        }
        if(parseCsvLine(line, (count == 0)) != 0){
            assert(false);
        }
        count+=1;
    }
    fs.close();

    // open and parse config file
    fs.open(config, std::fstream::in);
    while(std::getline(fs, line)){
        // ignore comment lines
        if(line[0] == '#'){
            count+=1;
            continue;
        }
        if(parseConfigLine(line)){
            assert(false);
        }
    }
    fs.close();

    // init special unit structure
    // add all buildings which should be considered
    // from the random list generator
    // index: 0: main, 1: worker, 2: vespene, 3: supply
    special["Prot"].push_back("Nexus");
    special["Prot"].push_back("Probe");
    special["Prot"].push_back("Assimilator");
    special["Prot"].push_back("Pylon");
    // TODO if this is a good approach -> add other races
}

std::string DataAcc::getAttributeString(std::string id, int attribute){
    std::string attr = data[id][attribute];
    if(attr == "Zer")
    {
        attr = "Zerg";
    }
    if(attr == "Pro")
    {
        attr = "Prot";
    }
    return attr;
}

std::vector<std::string> DataAcc::getAttributeVector(std::string id, int attribute){
    std::string attr = data[id][attribute];
    std::stringstream attrStream(attr);
    std::vector<std::string> result;
    std::string cell;

    while(std::getline(attrStream, cell, '/')){
        result.push_back(cell);
    }

    return result;
}

int DataAcc::getAttributeValue(std::string id, int attribute, bool fp){
    std::string attr = data[id][attribute];
    double value = 0;
    std::stringstream st(attr);
    st >> value;
    // work with fixed point precision
    // -> map from the double range to int by multiplying with 10000
    int ret = 0;
    if(fp){
        ret = (int)(value * FIXEDPOINT_FACTOR);
    } else {
        ret = (int)(value);
    }
    return ret;
}

int DataAcc::getParameter(std::string id, bool fp){
    std::string attr = settings[id];
    // parse to double
    double value = 0;
    std::stringstream st(attr);
    st >> value;
    // work with fixed point precision
    // -> map from the double range to int by multiplying with 10000
    int ret = 0;
    if(fp){
        ret = (int)(value * FIXEDPOINT_FACTOR);
    } else {
        ret = (int)(value);
    }
    return ret;
}

Unit DataAcc::getUnit(std::string name){

    bool unit = false;
    if((name == "Larva")||(name == "Injection")){
        unit = false;
    }else if(this->getAttributeString(name, DataAcc::structure) == "True"){
        unit = false;
    }else if(this->getAttributeString(name, DataAcc::structure) == "False"){
        unit = true;
    }else{
        assert(false);
    }


    Unit *u;

    int build_time = 0;
    int occ_limit = 0;
    int start_energy = 0;
    int max_energy = 0;
    if(name == "Injection")
    {
       build_time = 290000;
       occ_limit = 1;
       start_energy = 0;
       max_energy = 0;
       u = new Unit(name, build_time, occ_limit, unit, start_energy, max_energy, special_id);
       special_id++;
    }
    else
    {
    build_time = this->getAttributeValue(name, this->build_time, true);
    occ_limit = this->getAttributeValue(name, this->occupy_limit);
    start_energy = this->getAttributeValue(name, this->start_energy, true);
    max_energy = this->getAttributeValue(name, this->max_energy, true);
    if(name == "Larva")
    {
        build_time = 110000;
        occ_limit = 1;
        start_energy = 0;
        max_energy = 0;
    }
    u = new Unit(name, build_time, occ_limit, unit, start_energy, max_energy, cur_unit_id);
    }

    cur_unit_id ++;

    return *u;
}

