#include <iostream>
#include <fstream>
#include <string>
#include "tree.h"

Tree* parse(std::string expression);

using namespace std;

int main() {
    Tree* tree = new Tree("hello");
    cerr << tree << endl;

    freopen("log", "w", stderr);
    std::ifstream in("resources/in");

    std::string expression;
    getline(in, expression);
    std::cerr << "expression: " << expression << std::endl;

    Tree* res = parse(expression);
    std::cerr << "result: " << (res != nullptr) << std::endl;

    if (res != nullptr)
        res->print(std::cerr);

    return 0;
}

