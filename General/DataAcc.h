#include <string>
#include <unordered_map>
#include <vector>

#ifndef _DATAACC_H_
#define _DATAACC_H_

/*********************** Class DataAcc *************************
 * Parses a csv file with information about buildings and Units
 * stores them in Hash Map and allows access to the attributes
 * with the Names.
 * TODO: function which returns class for given name
 * TODO: also parse the parameters.csv file and implement functions
 ***************************************************************/

class DataAcc {
private:
    // stores the attributes for each unit
    std::unordered_map<std::string, std::vector<std::string>> data;
    // stores the name of the attributes
    std::vector<std::string> indices;
    // stores the names of the units and buildings
    std::vector<std::string> ids;
    // parses the csv lines and fills the data structures above
    int parseCsvLine(std::string, bool);
public:

    // enum for easy access to attributes
    enum Atr{
        //id,
        production_state = 0,
        minerals = 1,
        vespene = 2,
        supply = 3,
        supply_cost = 4,
        supply_provided = 5,
        build_time = 6,
        occupy_limit = 7,
        units_produced = 8,
        producer = 9,
        dependencies = 10,
        produces = 11,
        start_energy = 12,
        max_energy = 13,
        structure = 14,
        race = 15,
    };

    // constructor:
    // opens the given csv file and calls the parsing function for each line
    DataAcc(std::string);

    // function to return an attribute as string, vector or number
    // id = name of a building or unit
    // attribute = name of the desired attribute
    std::string getAttributeString(std::string id, int attribute);
    // separates the attribute string by '/' and returns all strings as vector
    std::vector<std::string> getAttributeVector(std::string id, int attribute);
    // tries to interpret the attribute as double value
    // TODO should use fixed point arithmetic
    double getAttributeValue(std::string id, int attribute);

    // some testing stuff
    std::vector<std::string> getIdVector() const {return {ids.begin(), ids.end()};}
    std::vector<std::string> getIndicesVector() const {return {indices.begin(), indices.end()};}
};

#endif // _DATAACC_H_
