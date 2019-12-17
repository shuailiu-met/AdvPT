#include "Race.h"
#include "Protoss/Protoss.h"
#include "General/DataAcc.h"
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>

using namespace std;

// definition for global id for Units
// See Unit.h for details
// TODO not really a good place for the definition -> change
int global_id = 0;

vector<string> readtxt(vector<string> a, string file){
    string buffer;
    fstream outFile;
    outFile.open(file,ios::in);
    while(getline(outFile, buffer)){
        a.push_back(buffer);
    }
    outFile.close();
    return a;
}

int main(int argc, char* argv[]){
    // needs to be called with race and build parameter filepath
    if(argc != 3){
        cout << "Usage: " << argv[0] << " <race> <buildorder>" << endl;
        exit(EXIT_FAILURE);
    }

    // read buildorder file
    vector<string> buildorder;
    buildorder = readtxt(buildorder, argv[2]);//read buildlist

    // get racename
    string race_name(argv[1]);

    // get the given race
    if(!race_name.compare("terran")){
        cout << "Use a Terran object!" << "\n";
    }else if(!race_name.compare("zerg")){
        cout << "Use a Zerg object!" << "\n";
    }else if(!race_name.compare("protoss")){
        Protoss prot(buildorder);
        int ret;
        while((ret = prot.advanceOneTimeStep()));
    }else{
        cout << "Unknown race!" << endl;
        exit(EXIT_FAILURE);
    }


    return 0;
}
