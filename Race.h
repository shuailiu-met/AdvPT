#pragma once
#include <list>
#include <string>
#include <iostream>
#include <algorithm>

#ifndef RACE_H_INCLUDED
#define RACE_H_INCLUDED


using namespace std;
int workers_start = 12;
int basis_start = 1;
int overlord_start = 1;
int larva_start = 3;
int minerals_start = 50;
int vespene_start = 0;
float mineral_harvesting = 0.7;
float vespene_harvesting = 0.63;
float energy_regen_rate = 0.5625;
float chronoboost_energy_cost = 50;
float chronoboost_speedup = 1.5;
int chronoboost_duration = 20;
float mule_energy_cost = 50;
int mule_duration = 64;
float mule_speedup = 3.8;
float injectlarvae_energy_cost = 25;
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
int supply_remaining = supply_limit_initial-supply_occupied_initial;

class Race
{
    public:
        string id;
        float mineral_cost;
        float vespene_cost;
        int supply;
        int supply_cost;
        int supply_produce;
        string producer;
        string dependency;
        string product;
        int units_produced;
        bool structure;
        string production_state;
        string racename;
        int building_time;
        int occupy_limit;
        float start_energy;
        float max_energy;

        bool Check_Resource();
        bool Check_Dependency();
        bool Check_Producer();

        Race(string i,string ps,float m,float v,int s,int sc,int sp,int bt,int ol,int up,string p,string d,string pr,float se,float me,bool st,string r):
            id(i),production_state(ps),mineral_cost(m),vespene_cost(v),supply(s),supply_cost(sc),supply_produce(sp),building_time(bt),occupy_limit(ol),units_produced(up),producer(p),dependency(d),product(pr),start_energy(se),max_energy(me),structure(st),racename(r)
            {};

        void Addontolist();
        void Update();
        void Worker_Distribution();
        void Producer_Consumed();


};


list<Race> RL;

bool Race::Check_Resource()
{
    if((supply_remaining>=supply_cost)&&(minerals>=mineral_cost)&&(vespene>=vespene_cost)){
            return true;
    }
    else{
            return false;
    }
};

bool Race::Check_Dependency()
{
    string target = this->dependency;
    if(target=="None")
    {
        return true;
    }
    else
    {
        auto it = find_if(RL.begin(),RL.end(),[&target](const Race &r){return (r.id == target);});
        if(it!=RL.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

bool Race::Check_Producer()
{
    if((this->racename == "Zerg")&&(this->structure==false))//Specialized for Zerg Unit due to the Larvae
    {
        if(num_larvae>0)
        {
            return true;
        }
        else{
            return false;
        }
    }
    else
    {
    string target = this->producer;
    if(target=="None")
    {
        return true;
    }
    else
    {
        auto it = find_if(RL.begin(),RL.end(),[&target](const Race &r){return (r.id == target);});
        if(it!=RL.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    }
};

void Race::Addontolist()
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
void Race::Update()
{
    supply_remaining += supply;
    minerals -= mineral_cost;
    vespene -= vespene_cost;
}



















/*void Addontolist(Race &r)
{
    if(r.dependency=="none")
    {
        ZL.push_back(r);
    }
    else{
    string target[2] = {r.dependency,"free"};
    auto it = find_if(ZL.begin(),ZL.end(),[&target](const Race &r){return (r.type == target[0])&&(r.status == target[1]);});
    cout << it->supply_cost <<endl;
    ZL.push_back(z);
    }
}*/

#endif // RACE_H_INCLUDED
