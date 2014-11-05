#include "deduction.h"

using namespace std;

deduction mainDeduction;
vector<string> expr;

int main() {
    expr.push_back("asdf");
    mainDeduction.doDeduction(expr);
    return 0;
}
