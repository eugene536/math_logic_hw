#include "checkOnFalse.h"
using namespace std;

unordered_map<string, bool> badAnswer;
vector<string> variables;
unordered_map<char, vector<string>[4]> lemms14;
vector<string> aOrNota;
static ifstream file;
vector< vector<string> > proof;
int curPermut;
string curContext;
deduction main_deduction;

void readLemms(string path, int cnt, char binOp) {
    vector<string> temp[4];
    string s = "axioms/" + path;
    int x, y, z;

    file.open(s.c_str(), ifstream::in);
    if (!file) {
        throw runtime_error("Can't open file " + s);
    }

    while (getline(file, s)) {
        if (!s.length())
            continue;
        if (s == "=") {
            file >> x >> y >> z;
            z = (x << 1) + y;
            continue;
        }
        temp[z].push_back(s);
    }

    for (int i = 0; i < cnt; i++)
        lemms14[binOp][i] = temp[i]; 
    file.close();
}

void init() {
    try {
        readLemms("not", 2, '!');
        readLemms("and", 4, '&');
        readLemms("or", 4, '|');
        readLemms("implication", 4, '-');

        //A | !A
        file.open("axioms/aOrNota", ifstream::in);
        string s;
        while (getline(file, s)) {
            aOrNota.push_back(s);
        }
    } catch(...) {
        file.close();
        throw;
    }
    file.close();
}

bool getBinOpValue(string s, bool x, bool y) {
    if (s == "&")
        return x & y;
    else if (s == "|")
        return x | y;
    else if (s == "->")
        return !x | y;
    else //if (s == "!")
        return !x;
}

void modifiedStrings(vector<string> const from, string const& left, string const& right, int offset) {
    int lst = proof.size() - 1;
    for (int i = 0; i < (int) from.size(); i++) {
        for (int j = 0; j < (int) from[i].size(); j++) {
            if (from[i][j] == 'A') {
                proof[lst][i + offset] += left;
            } else if (from[i][j] == 'B')  {
                proof[lst][i + offset] += right;
            } else {
                proof[lst][i + offset].push_back(from[i][j]);
            }
        }
    }
}

void setModifiedLemms(char binOp, int offset, string const& left, string const& right) {
    const vector<string> curLemms = lemms14[binOp][offset];
    proof.push_back(vector<string>(curLemms.size() + 1));

    //proof[lst][0] = curContext;
    modifiedStrings(curLemms, left, right, 1);
}

void mergeModifiedLemms() {
    int lst = proof.size() - 1;
    proof[lst].back();
}

bool getValue(parser::linkOnTree cur) {
    // for ! (not)
    if (!cur) 
        return false;

    if (goodCharForVar(cur->str[0])) {
        return (cur->valWholeTree = badAnswer[cur->str]);
    }

    bool leftVal = getValue(cur->left);
    bool rightVal = getValue(cur->right);
    if (cur->str == "!") {
         
    } else {
       setModifiedLemms(cur->str[0], (leftVal << 1) + rightVal, cur->left->exprSubTree, cur->right->exprSubTree);
    }


    return (cur->valWholeTree = getBinOpValue(cur->str, leftVal, rightVal));
}

bool isValidity(parser::linkOnTree cur) {
    int sz_v = variables.size();
    bool f = true;
    for (curPermut = 0; curPermut < (1 << sz_v); curPermut++) {
        curContext = "";
        //choose appropriate bits
        for (int j = 0; j < sz_v; j++) {
            badAnswer[variables[j]] = curPermut & (1 << j);

            //create newContext
            if (!badAnswer[variables[j]]) {
              curContext += "!";  
            }
            curContext += variables[j];
            if (j != sz_v - 1) {
                curContext += ",";
            } else {
                curContext += "|-";
            }
        }
        curContext += cur->exprSubTree;
        f = getValue(cur);
        if (!f)
            return false;

        //cerr << proof.size() - 1 << endl;
        main_deduction.doDeduction(proof.back());
        proof[proof.size() - 1] = main_deduction.result; 



        if (curPermut && (curPermut & (curPermut - 1)) == 0) {
            cerr << "merge" << endl;
            mergeModifiedLemms();
        }

    }
    return true;
}

//PRE: cur != NULL
void getVariables(parser::linkOnTree cur) {
    if (!cur->left && !cur->right) {
        if (badAnswer.count(cur->str) == 0) {
            badAnswer[cur->str] = 0;
            variables.push_back(cur->str);
        }
        return;
    }
    getVariables(cur->left);
    if (cur->right)
        getVariables(cur->right);
}

bool checkOnFalse(parser::linkOnTree cur) {
    init();
    getVariables(cur);
    bool f = isValidity(cur);
    if (!f) {
        for (auto it : variables)
            cout << it << " = " << badAnswer[it] << endl;
    }
    return f;
}
