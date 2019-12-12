#include <iostream>
#include <string>
#include <vector>
#include <iterator>


#include "DataAcc.h"
#include "BuildingsAndUnits.h"

using namespace std;

int main()
{
//We can use cin or read from buildinglist to get the name.
//Here in the test I set resource sufficient to test the producer and dependency
    BuildingsAndUnits *b = new BuildingsAndUnits("Drone");
    b->Addontolist();
    b = new BuildingsAndUnits("Hatchery");
    b->Addontolist();
    b = new BuildingsAndUnits("SpawningPool");
    b->Addontolist();
    return 0;
}
