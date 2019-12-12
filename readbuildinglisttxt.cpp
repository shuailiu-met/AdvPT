//
//  readbuildinglisttxt.cpp
//  SC2-2
//
//  Created by 宋經緯 on 2019/12/12.
//  Copyright © 2019 宋經緯. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>


#include "DataAcc.h"
#include "BuildingsAndUnits.h"

using namespace std;

vector<string> readtxt(vector<string> a)
{
    char buffer[128];
    fstream outFile;
    outFile.open(".../buildlists/terran/terran2.txt",ios::in);
    while(!outFile.eof())
    {
        outFile.getline(buffer,128,'\n');
        a.push_back(buffer);
        cout<<buffer<<endl;
    }
    outFile.close();
    return a;
}
int main()
{
     vector<string> Buildlist;
    Buildlist = readtxt(Buildlist);//read buildlist
    for (vector<string>::iterator it = Buildlist.begin(); it != Buildlist.end(); ++it) {
        Build(*it);
    }
    return 0;
}
