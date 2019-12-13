#ifndef PROTOSS_H
#define PROTOSS_H

#include "../Race.h"

class Protoss: public Race{
private:
public:
    Protoss(std::vector<std::string> buildorder) : Race{buildorder}{}

};
#endif
