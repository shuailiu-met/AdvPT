#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "DataAcc.h"

using namespace std;

int main(){
    DataAcc data("../res/unit_db.csv");

    // get all unit names and all attribute names as vector
    vector<string> id = data.getIdVector();
    vector<string> in = data.getIndicesVector();

    // print all attributes for all units
    cout << "Print all attribute values:" << endl;
    for(vector<string>::iterator it = id.begin(); it != id.end(); ++it) {
        cout << *it << " : " << endl;
        for(int i = DataAcc::production_state; i <= DataAcc::race; i++) {
            string attrVal = data.getAttributeString(*it, i);
            cout << "\t" << in[i] << " : " << attrVal << "\n";
        }
    }

    cout << endl;

    // get dependency/produces vectors
    vector<string> deps = data.getAttributeVector("Stargate", DataAcc::dependencies);
    cout << "Stargate Dependencies: " << '\n';
    for(vector<string>::iterator it = deps.begin(); it != deps.end(); ++it) {
        cout << '\t' << *it << '\n';
    }
    deps = data.getAttributeVector("RoboticsFacility", DataAcc::produces);
    cout << "RoboticsFacility produces: " << '\n';
    for(vector<string>::iterator it = deps.begin(); it != deps.end(); ++it) {
        cout << '\t' << *it << '\n';
    }
    deps = data.getAttributeVector("Pylon", DataAcc::dependencies);
    cout << "Pylon Dependencies: " << '\n';
    for(vector<string>::iterator it = deps.begin(); it != deps.end(); ++it) {
        cout << '\t' << *it << '\n';
    }

    // get values as double
    double val = data.getAttributeValue("VoidRay", DataAcc::supply_cost);
    cout << "VoidRay supply cost: " << val << endl;
    val = data.getAttributeValue("Baneling", DataAcc::supply);
    cout << "Baneling supply: " << val << endl;
    return 0;
}
