#ifndef DATAACC_H
#define DATAACC_H

#include <string>
#include <unordered_map>
#include <vector>

#define FIXEDPOINT_FACTOR 10000

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
    // stores the global settings
    std::unordered_map<std::string, std::string> settings;
    // stores the name of the attributes
    std::vector<std::string> indices;
    // stores the names of the units and buildings
    std::vector<std::string> ids;
    // parses the csv lines and fills the data structures above
    int parseCsvLine(std::string, bool);
    // parses the config file lines and fills the data structures above
    int parseConfigLine(std::string);
public:

    // enum for easy access to attributes
    enum Atr{
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

/*
    enum Params{
        workers_start = "WORKERS_START",
        basis_start = "BASIS_START",
        oerlord_start = "OERLORD_START",
        larva_start = "LARVA_START",
        minerals_start = "MINERALS_START",
        vespene_start = "VESPENE_START",
        mineral_harvesting = "MINERAL_HARVESTING",
        vespene_harvesting = "VESPENE_HARVESTING",
        energy_regen_rate = "ENERGY_REGEN_RATE",
        chronoboost_energy_cost = "CHRONOBOOST_ENERGY_COST",
        chronoboost_speedup = "CHRONOBOOST_SPEEDUP",
        chronoboost_duration = "CHRONOBOOST_DURATION",
        mule_energy_cost = "MULE_ENERGY_COST",
        mule_duration = "MULE_DURATION",
        mule_speedup = "MULE_SPEEDUP",
        injectlarvae_energy_cost = "INJECTLARVAE_ENERGY_COST",
        injectlarvae_duration = "INJECTLARVAE_DURATION",
        injectlarvae_amount = "INJECTLARVAE_AMOUNT",
        max_injectlarvae_per_building = "MAX_INJECTLARVAE_PER_BUILDING",
        larva_duration = "LARVA_DURATION",
        max_larva_per_building = "MAX_LARVA_PER_BUILDING",
    };*/

    // constructor:
    // opens the given csv file and calls the parsing function for each line
    DataAcc(std::string unitdb, std::string parameters);

    // function to return an attribute as string, vector or number
    // id = name of a building or unit
    // attribute = name of the desired attribute
    std::string getAttributeString(std::string id, int attribute);
    // separates the attribute string by '/' and returns all strings as vector
    std::vector<std::string> getAttributeVector(std::string id, int attribute);
    // tries to interpret the attribute as double value and returns that value
    // as integer multiplied by FIXEDPOINT_FACTOR
    int getAttributeValue(std::string id, int attribute);
    // returns the setting as int value multiplied by FIXEDPOINT_FACTOR
    int getParameter(std::string setting);

    // some testing stuff
    std::vector<std::string> getIdVector() const {return {ids.begin(), ids.end()};}
    std::vector<std::string> getIndicesVector() const {return {indices.begin(), indices.end()};}
};

#endif // DATAACC_H
