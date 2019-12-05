#include<iostream>
#include "Zerg.h"



int main()
{
    Zerg z0("Worker","none",0,"occupied");
    Addontolist(z0);
    Zerg z4("Worker","none",10,"free");
    Addontolist(z4);
    Zerg z1("sp","Worker",20,"free");
    Addontolist(z1);
    Zerg z2("zg","sp",30,"free");
    Addontolist(z2);
    return 0;
}
