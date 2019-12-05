#pragma once
#include <list>
#include <string>
#include <iostream>
#include <algorithm>

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
//double chronoboost_energy_cost = 50;
//double chronoboost_speedup = 1.5;
//int chronoboost_duration = 20;
//double mule_energy_cost = 50;
//int mule_duration = 64;
//double mule_speedup = 3.8;
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

int minerals = 50;
int vespene = 0;
int supply_occupied_initial = 12;//8+6-12
int supply_limit_initial = 14;
int num_larvae = 3;
int num_worker = 12;


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
        string status;

        /*static int minerals;
        static int vespene;
        static int supplyremaining;
        static int num_larvae;
        static int num_worker;*/
        Zerg(string,string,int,string);
        //bool dependencycheck();
        //bool check(Zerg &);
        //bool operator()(const Zerg &) const;
        //bool checklarvae();
        //bool checkworker();//use workerlist.length();
        //bool checksupply();
        //bool checkdependency();
};
  // use find_if func
list<Zerg> ZL;
Zerg::Zerg(string n,string d,int s,string st):type(n),dependency(d),supply_cost(s),status(st)
{
    cout << "Zerg" << endl;
}

/*bool check(Zerg &z)
{
    return (this->dependency == z.type);
}*/

/*bool dependencycheck(string d)
{
    auto it = find_if(ZL.begin(),ZL.end(),[d](const Zerg& zerg) { return zerg.type == d;});
    cout<< it->dependency;
    return true;
    else
    {
        return false;
        cout << "f";
    }
}

void Addontolist(Zerg &z)
{
if(dependencycheck(z.dependency)==true)
    {
        //num_worker = num_worker-1;
        ZL.push_back(z);
        cout << "Built";
}
else
{
    cout << "f";
}
}*/

void Addontolist(Zerg &z)
{
    if(z.dependency=="none")
    {
        ZL.push_back(z);
    }
    else{
    string target[2] = {z.dependency,"free"};
    auto it = find_if(ZL.begin(),ZL.end(),[&target](const Zerg &z){return (z.type == target[0])&&(z.status == target[1]);});
    cout << it->supply_cost <<endl;
    ZL.push_back(z);
    }
}






/*class Worker:protected Zerg
{
    private:
        bool harvest_status;//0 for gas,1 for minerals
    public:
        status();
        {
            return harvest_status;
        }

};
list<Worker> workerlist;







class Building:protected Zerg
{
    //friend class listofbuilt;
    private:


    public://read info from CSV file required in this class
        Building()
        {};
        Buildingcheck()
        {
            return true;
        };

};

list<Building> BL;

void Addontolist(Building &b)
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
        Unit(){}
        Unitcheck()
        {
            return true;
        }

};

list<Unit> UL;

void Addontolist(Unit &u)
{
if(u.Unitcheck()==true)
{
    num_larvae = num_larvae-1;
    UL.push_front(u);
    cout << "produced";
}
}



class Queen:protected Unit
{
    private:

    public:

};

list<Queen> QL;*/
#endif // ZERG_H_INCLUDED
