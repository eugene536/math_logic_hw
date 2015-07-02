#include "checkOnFalse.h"
#include "deduction.h"
#include <ctime>

using namespace std;
extern std::vector< std::vector<std::string> > proof;

int main() {
    ios_base::sync_with_stdio(false);
    #ifdef DEBUG
        ifstream in = ifstream("in");
        freopen("out", "w", stdout);
    #else
        istream &in = cin;
    #endif

    parser main_parser;
    double start = clock();

    deduction mainDeduction;
    vector<string> const expr;

    string s;
    getline(in, s);
    if (!s.length()) {
        cerr << endl << "empty expression" << endl;
        return 0;
    }

    checkOnFalse(main_parser.parse(s));

    double end = clock();
    cerr << (end - start) / CLOCKS_PER_SEC << endl;
    return 0;
}
