#include "checkOnFalse.h"
#include "deduction.h"

using namespace std;
string s;
parser main_parser;
parser::linkOnTree expression;
ifstream file;
extern std::vector< std::vector<std::string> > proof;

deduction mainDeduction;
vector<string> const expr;

int main() {
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

    expression = main_parser.parse(s);
    if (!checkOnFalse(expression)) {
        return 0;
    }

    cerr << "TRUE" << endl;

    for (int i = 0; i < (int) proof.size(); i++) {
        for (int j = 0; j < (int) proof[i].size(); j++) {
            cout << proof[i][j] << endl;
        }    
        cout << endl << endl;
    }

    delete expression;
    file.close();
    return 0;
}
