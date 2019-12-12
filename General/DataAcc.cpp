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
    std::cout<<line<<"\n";

    int count = 0;
    while(std::getline(lineStream, cell, ',')){
        // the first element is the name of the unit
        // -> will be the id for unit access
        std::cout<<cell<<"\n";
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
        std::cout<<"Broken!!! " << count << "\n";
        return -1;
    }
    return 0;
}


DataAcc::DataAcc(std::string unitdb, std::string config){
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

int DataAcc::getAttributeValue(std::string id, int attribute){
    std::string attr = data[id][attribute];
    double value = 0;
    std::stringstream st(attr);
    st >> value;
    // work with fixed point precision
    // -> map from the double range to int by multiplying with 10000
    int ret = (int)(value * FIXEDPOINT_FACTOR);
    return ret;
}

int DataAcc::getParameter(std::string id){
    std::string attr = settings[id];
    // parse to double
    double value = 0;
    std::stringstream st(attr);
    st >> value;
    // work with fixed point precision
    // -> map from the double range to int by multiplying with 10000
    int ret = (int)(value * FIXEDPOINT_FACTOR);
    return ret;
}
