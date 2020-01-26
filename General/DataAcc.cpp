#include "DataAcc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <type_traits>
#include <algorithm>
#include <cstdlib>
//producer also need this
void DataAcc::recursiveDependencyHelper(std::vector<std::string> *bo, std::string dep){
    // if dependency exists - return
    if(std::count(bo->begin(), bo->end(), dep))
        return;

    // we always satisfy the main building dependency
    if(dep == "Hatchery" || dep == "Nexus" || dep == "CommandCenter")
        return;

    // add all dependencies for current dep first
    std::vector<std::string> deps = this->getAttributeVector(dep, dependencies);
    for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
        this->recursiveDependencyHelper(bo, *it);
    }

    // add the given dependency
    bo->push_back(dep);
}

//TODO:only add one here
void DataAcc::recursiveProducerHelper(std::vector<std::string> *bo, std::string prod){
    // if producer exists - return
    if(std::count(bo->begin(), bo->end(), prod))
    return;

    // we always satisfy the main building product
    if(prod == "Hatchery" || prod == "Nexus" || prod == "CommandCenter")
        return;

    // add all producer for current producer first
    std::vector<std::string> producers = this->getAttributeVector(prod, producer);
    // here we only use the first of producers, since our goal is to be fast, so basic producer is considered
    std::vector<std::string>::iterator firstprod = producers.begin();
    if(*firstprod == "Larva")
    {
        return;
    }

    this->recursiveProducerHelper(bo, *firstprod);
    // add the given producer
    bo->push_back(prod);

}

std::vector<std::string> DataAcc::getRandomBuildorder(std::string race, std::string target, int count){
    std::vector<std::string> bo;

    // add the neccessary dependencies for our target + target itself
    this->recursiveDependencyHelper(&bo, target);
    //producer needed to be consider
    this->recursiveProducerHelper(&bo,target);
    std::string race_now = this->getAttributeString(target,race);
    bool vespeneexist = false;

        if(race_now == "Zerg"){
        std::string vespenebuilding = "Extractor";
        for(std::vector<std::string>::iterator it = bo->begin();it!=bo->end();it++){
        if(*it == vespenebuilding)
        {
            vespeneexist = true;
            break;
        }
        }
        }
        else if(race_now == "Terr")
        {
        std::string vespenebuilding = "Refinery";
        for(std::vector<std::string>::iterator it = bo->begin();it!=bo->end();it++){
        if(*it == vespenebuilding)
        {
            vespeneexist = true;
            break;
        }
        }

        }
        else if(race_now == "Prot")
        {
        std::string vespenebuilding = "Assimilator";
        for(std::vector<std::string>::iterator it = bo->begin();it!=bo->end();it++){
        if(*it == vespenebuilding)
        {
            vespeneexist = true;
            break;
        }
        }
        }
        else
        {
            assert(false);
        }

        if(vespeneexist==false)
        {
        for(std::vector<std::string>::iterator it2 = bo->begin();it2!=bo->end();it2++){
        if(this->getAttributeValue(name,vespene,true)!=0)
        {
            if(race_now == "Zerg")
            {
                std::string addvespenebuilding = "Extractor";
                bo->push_back(addvespenebuilding);
            }
            else if(race_now == "Terr")
            {
                std::string addvespenebuilding = "Refinery";
                bo->push_back(addvespenebuilding);
            }
            else if(race_now == "Prot")
            {
                std::string addvespenebuilding = "Assimilator";
                bo->push_back(addvespenebuilding);
            }
            break;
        }
        }
        }

    // for quick array access, hardcode the race indices
    // NEEDS TO BE CHANGED IF CSV FILE CHANGES!!!
    // indices: 0-36 = zerg, 37-74 terran, 75-107 protoss
    int off = 0, size = 0;
    if(!race.compare("Zerg")){
        off = 0;
        size = 37;
    }else if(!race.compare("Terr")){
        off = 37;
        size = 38;
    }else if(!race.compare("Prot")){
        off = 75;
        size = 33;
    }else{
        assert(false);
    }

    // add the given amount of Units
    uint32_t to_add = count - bo.size();
    for(uint32_t i = 0; i < to_add; i ++){
        bool added = false;
        while(!added){
            // get the random unit
            std::string unit = ids[rand() % size + off];
            // check if needed dependencies exists
            std::vector<std::string> deps = this->getAttributeVector(unit, dependencies);
            for(std::vector<std::string>::iterator it = deps.begin(); it != deps.end(); it++) {
                // TODO ist it better to add the dependencies for this unit
                // or to generate another random unit?
                // If not fount generate another random Unit
                if(!std::count(bo.begin(), bo.end(), *it))
                    continue;
            }

            bo.push_back(unit);
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

