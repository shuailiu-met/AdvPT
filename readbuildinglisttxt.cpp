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

void readtxt(string a[])
{
    char buffer[128];
    int i=0;
    fstream outFile;
    outFile.open("...buildlists/terran/terran1.txt",ios::in);
    while(!outFile.eof())
    {
        outFile.getline(buffer,128,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
        a[i]=buffer;
        cout<<buffer<<endl;
        ++i;
    }
    outFile.close();
}
int main()
{
    string a[10];
    readtxt(a);
    for (int i=0; i<10; i++) {
    BuildingsAndUnits *b = new BuildingsAndUnits(a[i]);
    b->Addontolist();
    }
}
