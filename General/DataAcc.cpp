#include "DataAcc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <type_traits>
#include <algorithm>


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
    if((name == "Larva")){
        unit = false;
    }
    /*else if(name == "Injection")
    {
        unit = false;
    }*/
    else if(this->getAttributeString(name, DataAcc::structure) == "True"){
        unit = false;
    }else if(this->getAttributeString(name, DataAcc::structure) == "False"){
        unit = true;
    }else{
        assert(false);
    }

    Unit *u;
    int build_time = this->getAttributeValue(name, this->build_time, true);
    int occ_limit = this->getAttributeValue(name, this->occupy_limit);
    int start_energy = this->getAttributeValue(name, this->start_energy, true);
    int max_energy = this->getAttributeValue(name, this->max_energy, true);
    if(name == "Larva")
    {
        build_time = 110000;
        occ_limit = 1;
        start_energy = 0;
        max_energy = 0;
    }
    u = new Unit(name, build_time, occ_limit, unit, start_energy, max_energy, cur_unit_id);

    cur_unit_id ++;

    return *u;
}

