#include <list>
#include <string>
#include <iostream>
#include <algorithm>

#include "DataAcc.h"

#ifndef _BUILDINGSANDUNITS_H_
#define _BUILDINGSANDUNITS_H_

using namespace std;
int minerals = 5000;
int vespene = 1000;
int supply_occupied_initial = 12;//8+6-12
int supply_limit_initial = 14;
int num_larvae = 3;
int num_worker = 12;
//int supply_remaining = supply_limit_initial-supply_occupied_initial;
int supply_remaining = 1000;


class BuildingsAndUnits
{
    public:
        string id;
        /*double mineral_cost;
        double vespene_cost;
        double supply;
        double supply_cost;
        double supply_produce;
        string producer;
        string dependency;
        string product;
        double units_produced;
        bool structure;
        string production_state;
        string racename;
        double building_time;
        double occupy_limit;
        double start_energy;
        double max_energy;*/

        bool Check_Resource();
        bool Check_Dependency();
        bool Check_Producer();
        //string Get_Producer();

        BuildingsAndUnits(string i):id(i)
        {};
        void Addontolist();
        void Update();
        void Worker_Distribution();
        void Producer_Consumed();

};


list<BuildingsAndUnits> RL;

DataAcc data("../res/unit_db.csv");

// get all unit names and all attribute names as vector
vector<string> id = data.getIdVector();
vector<string> in = data.getIndicesVector();


bool BuildingsAndUnits::Check_Resource()
{
    double scost = data.getAttributeValue(this->id,DataAcc::supply_cost);
    double vcost = data.getAttributeValue(this->id,DataAcc::vespene);
    double mcost = data.getAttributeValue(this->id,DataAcc::minerals);

    if((supply_remaining>=scost)&&(minerals>=mcost)&&(vespene>=vcost)){
            return true;
    }
    else{
            return false;
    }
};

bool BuildingsAndUnits::Check_Dependency()
{
    string deps = data.getAttributeString(this->id, DataAcc::dependencies);
    //cout << deps;
    if(deps.length()==0)//if the string is empty, then no dependency
    {
        return true;
    }
    else
    {
        auto answer = find_if(RL.begin(),RL.end(),[&deps](const BuildingsAndUnits &b){return (b.id == deps);});
        if(answer!=RL.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

bool BuildingsAndUnits::Check_Producer()
{
    vector<string> prod = data.getAttributeVector(this->id, DataAcc::producer);
    string racename = data.getAttributeString(this->id, DataAcc::race);
    string structure = data.getAttributeString(this->id, DataAcc::structure);
    if((racename == "Zer")&&(structure == "False")&&(num_larvae>0))//Specialized for Zerg Unit due to the Larvae
    {
        return true;
    }
    else
    {
    vector<string> prod = data.getAttributeVector(this->id, DataAcc::producer);
    string get;
    for(vector<string>::iterator it = prod.begin(); it != prod.end(); ++it) {
        string target = *it;
        auto answer = find_if(RL.begin(),RL.end(),[&target](const BuildingsAndUnits &b){return (b.id == target);});
        if(answer!=RL.end())
        {
            get = target;//May have problem here if directly return true of false(maybe due to the loop)
        }
        else
        {
            get = "No";
        }
    }
    if (get=="No")
    {
        return false;
    }
    else
    {
        return true;
    }
    }
};

void BuildingsAndUnits::Addontolist()
{
    if((Check_Dependency()==true)&&(Check_Producer()==true)&&(Check_Resource()==true))
    {
        RL.push_back(*this);
        cout << "pushed" << endl;
        Update();
    }
    else
    {
        cout << "not pushed" <<endl;
    }

};

//Update Resource after successfully building order set
void BuildingsAndUnits::Update()
{
    double scost = data.getAttributeValue(this->id,DataAcc::supply_cost);
    double vcost = data.getAttributeValue(this->id,DataAcc::vespene);
    double mcost = data.getAttributeValue(this->id,DataAcc::minerals);
    supply_remaining -= scost;
    minerals -= mcost;
    vespene -= vcost;
}


#endif // _BUILDINGSANDUNITS_H_
