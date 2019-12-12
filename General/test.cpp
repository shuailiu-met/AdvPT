#include <iostream>
#include <string>
#include <vector>
#include <iterator>


#include "DataAcc.h"
#include "BuildingsAndUnits.h"

using namespace std;

//Initialize the Buildings and Units which is existed at beginning, put them into the list
void Initializer(string race)
{
    if(race == "Zerg")
    {
    BuildingsAndUnits *b = new BuildingsAndUnits("Hatchery");
    b->Initial_Set_OnTo_List();
    b = new BuildingsAndUnits("Overload");
    b->Initial_Set_OnTo_List();
    for(int i = 0;i<12;i++)
    {
    b = new BuildingsAndUnits("Drone");
    b->Initial_Set_OnTo_List();
    }
    }


    else if(race == "Terran")
    {
    BuildingsAndUnits *b = new BuildingsAndUnits("CommandCenter");
    b->Initial_Set_OnTo_List();
    for(int i = 0;i<12;i++)
    {
    b = new BuildingsAndUnits("SCV");
    b->Initial_Set_OnTo_List();
    }
    }


    else if(race == "Protoss")
    {
    BuildingsAndUnits *b = new BuildingsAndUnits("Nexus");
    b->Initial_Set_OnTo_List();
    for(int i = 0;i<12;i++)
    {
    b = new BuildingsAndUnits("Probe");
    b->Initial_Set_OnTo_List();
    }
    }

    else
    {
        cout << "Please input correctly" << endl;
    }
}

//Build a new Unit, if it can be put into the list(the function will return 1), then put. If the push failed, then delete the object.
void Build(string i)
{
    BuildingsAndUnits *b = new BuildingsAndUnits(i);
    bool success = b->Push_And_Check();
    if(success==false)
    {
        delete b;
    }
}

int main()
{
    //We can use cin or read from buildinglist to get the name.
    //Here in the test I set resource sufficient to test the producer and dependency
    string race;
    cout << "Choose your Race:";
    cin >> race;
    Initializer(race);//Initializing
    string i;
    while(i!="end")
    {
    cin >> i;
    Build(i);//Build new object
    }

    return 0;
}



