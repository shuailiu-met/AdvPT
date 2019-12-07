#include<iostream>
#include <string>
#include <vector>
#include <iterator>

#include "Race.h"

int main()
{
//by changing number of Larva or minerals or supplies to 0,not pushed
Race *r1= new Race("Worker","12",50,0,-1,1,0,10,1,1,"Larva","None","/",0,0,false,"Zerg");
r1->Addontolist();
Race *r2= new Race("SP","12",100,0,0,0,0,10,1,1,"Worker","None","/",0,0,true,"Zerg");
r2->Addontolist();
Race *r3= new Race("Zergling","12",50,0,-2,2,0,10,1,1,"Larva","SP","000",0,0,false,"Zerg");
r3->Addontolist();
return 0;
}
