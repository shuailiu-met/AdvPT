#pragma once
#include <list>
#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

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
    private:
       //string type;
       //int supply_cost;
       //int supply_produced;
       //int build_time;
       //int occupied_time;
       //int unit_produced;
       //string producer;
       //string dependency;
       //string product;
       //int start_energy;
       //int max_energy;
       //bool structure;
       Zerg *test;

    public:
        static int minerals;
        static int vespene;
        static int supplyremaining;
        Zerg();
        checklarvae();
        checksupply();
};

int Zerg::minerals = 50;
int Zerg::vespene = 0;
int Zerg::supplyremaining = 2;//8+6-12

class Build:protected Zerg
{
    friend class listofbuilt;
    private:
       string type;
       int supply_cost;
       int supply_produced;
       int build_time;
       int occupied_time;
       int unit_produced;
       string producer;
       string dependency;
       string product;
       int start_energy;
       int max_energy;
       bool structure;

    public://read info from CSV file required in this class

};

class listofbuilt
{

};
#endif // ZERG_H_INCLUDED
