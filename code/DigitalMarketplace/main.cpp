/*
PROGRAM INTENT: the objective of this program is to simulate a marketplace where users can buy and sell items. 
The program provides a front-end interface for users to interact with the marketplace, allowing them to view available items, make purchases, and manage their accounts. 
The main function initializes the front-end and starts the execution of the marketplace program.

All classes are described in their respective header files

the program should be run via the project solution, where then the 4 cpp files and 3 header files are compiled and the user terminal is used to interact with the program. 
commands found by entering the help command.
*/

#include <iostream>
#include "front.h"
#include "market.h"
#include "user.h"

using namespace std;
//runs the front end of the marketplace program
int main() {
    FrontEnd frontEnd(cin, cout);
    frontEnd.executeFrontEnd();
    return 0;
}
