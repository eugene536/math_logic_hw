#include <iostream>
#include "deduction.h"

using namespace std;

string s;
vector<string> expr;
deduction mainDeduction;

int main() {
    ios_base::sync_with_stdio(false);
    #ifdef DEBUG
    freopen("in", "r", stdin);
    freopen("out", "w", stdout);
    #endif

    while (getline(cin, s)) {
        if (!s.length())
            continue;
        expr.push_back(s);
    }

    mainDeduction.doDeduction(expr);
    for (auto it : mainDeduction.result)
        cout << it << endl;

    return 0;
}
