#include "checkOnFalse.h"
#include "deduction.h"
#include <ctime>

using namespace std;
string s;
parser main_parser;
parser::linkOnTree expression;
ifstream file;
extern std::vector< std::vector<std::string> > proof;

deduction mainDeduction;
vector<string> const expr;

int main() {
    double start = clock();
    ios_base::sync_with_stdio(false);

    #ifdef DEBUG
    file.open("in", ifstream::in);
    freopen("out", "w", stdout);
    #endif

    getline(file, s);
    if (!s.length()) {
        cerr << endl << "empty expression" << endl;
        return 0;
    }

    checkOnFalse(main_parser.parse(s));

    delete expression;
    #ifdef DEBUG
    file.close();
    #endif

    double end = clock();
    cerr << (end - start) / CLOCKS_PER_SEC << endl;
    return 0;
}
