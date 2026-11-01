#include <fstream>
#include <iostream>
#include "front.h"

using namespace std;

//accept file paths from command line arguments
int main(int argumentcount, char* arguments[]) {
    if (argumentcount != 5) {
        cerr << "usage: marketplace accounts games collections transactions\n";
        return 1;
    }
    //check input files before starting
    for (int fileindex = 1; fileindex <= 3; fileindex++) {
        ifstream inputfile(arguments[fileindex]);
        if (!inputfile.is_open()) {
            cerr << "could not open input file: " << arguments[fileindex] << "\n";
            return 1;
        }
        if (string(arguments[fileindex]) == arguments[4]) {
            cerr << "transaction output must have a separate path\n";
            return 1;
        }
    }
    //prepare a fresh transaction output file
    ofstream transactionfile(arguments[4]);
    if (!transactionfile.is_open()) {
        cerr << "could not open transaction output\n";
        return 1;
    }
    transactionfile.close();
    FrontEnd frontend(cin, cout, arguments[1], arguments[2], arguments[3], arguments[4]);
    frontend.executeFrontEnd();
    return 0;
}
