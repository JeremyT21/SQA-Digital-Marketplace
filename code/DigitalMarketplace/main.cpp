#include <iostream>
#include "front.h"
#include "market.h"
#include "user.h"

using namespace std;

int main() {
    FrontEnd frontEnd(cin, cout);
    frontEnd.executeFrontEnd();
    return 0;
}
