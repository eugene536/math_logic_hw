#include <iostream>
#include <fstream>
#include <string>
#include "kripke.h"
#include "parser.h"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    std::cerr << "task5" << std::endl;

    Kripke kripke("in");
    kripke.start("out");
    std::cerr << "OK" << std::endl;

    return 0;
}

