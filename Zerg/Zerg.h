#ifndef ZERG_H_INCLUDED
#define ZERG_H_INCLUDED

#include "Race.h"
#include <iostream>

Class Zerg: public Race{
private:
public:
    int workersOnVespene();
}

int Zerg::workersOnVespene()
{
    string target = "Extractor";
    auto it = finished.begin();
    int i = 0;
    while(it!=finished.end())
    {
    it = find_if(it,finished.end(),[&target](const Unit &u){return (u.getName() == target);});
    if(it!=finished.end())
    {
        i++;
        it++;
    }
    }
    return i;
}



#endif // ZERG_H_INCLUDED
