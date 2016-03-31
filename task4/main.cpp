#include <iostream>
#include <fstream>
#include <string>
#include "deduction.h"
#include "parser.h"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    std::cerr << "task4" << std::endl;

    Deduction deduction("in");
    deduction.doDeduction("out");

    return 0;
}

