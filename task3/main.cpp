#include "checkOnFalse.h"
#include <string.h>
#include <fstream>

using namespace std;
string s;
parser main_parser;
parser::linkOnTree expression;
unordered_map<char, vector<string>[4]> lemms14;
vector<string> aOrNota;
ifstream file;

void readLemms(string path, int cnt, char binOp) {
    vector<string> temp[4];
    string s = "axioms/" + path;
    int x, y, z;

    file.open(s.c_str(), ifstream::in);
    if (!file) {
        throw runtime_error("Can't open file " + s);
    }

    while (getline(file, s)) {
        if (s == "=") {
            file >> x >> y >> z;
            z = (x << 1) + y;
            cerr << "==" << z << endl;
            continue;
        }
        temp[z].push_back(s);
    }
    for (int i = 0; i < cnt; i++)
        lemms14[binOp][i] = temp[i]; 
    file.close();
}

void init() {
    readLemms("not", 2, '!');
    readLemms("and", 4, '&');
    readLemms("or", 4, '|');
    readLemms("implication", 4, '-');

    file.open(s.c_str(), ifstream::in);
    string s;
    while (getline(file, s)) {
        aOrNota.push_back(s);
    }
    file.close();
}

int main() {
    ios_base::sync_with_stdio(false);
    cerr << "bef init" << endl;
    init();

    #ifdef DEBUG
    file.open("in", ifstream::in);
    //freopen("out", "w", stdout);
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

    cout << "TRUE" << endl;
    file.close();

    return 0;
}
