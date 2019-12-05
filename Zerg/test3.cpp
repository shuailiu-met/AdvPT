
#include <list>

#include <string>
#include <iostream>

#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED
using namespace std;
//default values for Zerg
int workers_start = 12;
int basis_start = 1;
int overlord_start = 1;
int larva_start = 3;
int minerals_start = 50;
int vespene_start = 0;
double mineral_harvesting = 0.7;
double vespene_harvesting = 0.63;
double energy_regen_rate = 0.5625;
double chronoboost_energy_cost = 50;
double chronoboost_speedup = 1.5;
int chronoboost_duration = 20;
double mule_energy_cost = 50;
int mule_duration = 64;
double mule_speedup = 3.8;
double injectlarvae_energy_cost = 25;
int injectlarvae_duration = 29;
int injectlarvae_amount = 3;
int max_injectlarvae_per_building = 19;
int larvae_duration = 11;
int max_larvae_per_building = 3;

//producer_consumed_at_start - 0;
//producer_consumed_at_end - 1;
//producer_occupied - 2;
//for pair
//use list

class Zerg
{
    public:
        string type;
        int supply_cost;
        int supply_produced;
        int build_time;
        int occupied_time;
        int occupy_limit;
        int unit_produced;
        string producer;
        string dependency;
        string product;
        int start_energy;
        int max_energy;
        bool structure;//false for unit,true for building
        Zerg *test;

        static int minerals;
        static int vespene;
        static int supplyremaining;
        static int num_larvae;
        static int num_worker;

        bool checklarvae();
        bool checkworker();
        bool checksupply();
        bool checkdependency();
};

bool Zerg::checkdependency()
{
    return true;
}

bool Zerg::checkworker()
{
    return true;
}

int Zerg::minerals = 50;
int Zerg::vespene = 0;
int Zerg::supplyremaining = 2;//8+6-12
int Zerg::num_larvae = 3;
int Zerg::num_worker = 12;

class Building:protected Zerg
{
    //friend class listofbuilt;
    private:


    public://read info from CSV file required in this class
        Building()
        {

        };

        Buildingcheck()
        {
            if((checkdependency()==true)&&(checkworker()==true)){

            }
            return true;
        };
        Addontolist(Building &);
};

list<Building> BL;

Building::Addontolist(Building &b)
{
if(b.Buildingcheck()==true)
    {
        num_worker = num_worker-1;
        BL.push_front(b);
        cout << "Built";
}
}

class Unit:protected Zerg
{
    private:

    public:
        Unit()
        {
            if((checkdependency()==1)&&(checklarvae()==1)&&(checksupply()==1)){

            }
            num_larvae = num_larvae - 1;
        }

};

list<Unit> UL;




class Queen:protected Unit
{
    private:

    public:

};

list<Queen> QL;
#endif


int main()
{
    Building b1;
    b1.Addontolist(b1);
    return 0;
}
