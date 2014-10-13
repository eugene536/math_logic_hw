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
string resultProofExpr;

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

void modifiedStrings(vector<string> const from, vector<string>* to, string const& left, string const& right, int offset) {
    for (int i = 0; i < (int) from.size(); i++) {
        for (int j = 0; j < (int) from[i].size(); j++) {
            if (from[i][j] == 'A') {
                (*to)[i + offset] += left;
            } else if (from[i][j] == 'B')  {
                (*to)[i + offset] += right;
            } else {
                (*to)[i + offset].push_back(from[i][j]);
            }
        }
    }
}

void setModifiedLemms(char binOp, int offset, string const& left, string const& right) {
    const vector<string> curLemms = lemms14[binOp][offset];
    int lst = proof.size() - 1;
    int prevProofSize = proof[lst].size();
    proof[lst].resize(prevProofSize + curLemms.size());

    modifiedStrings(curLemms, &proof[lst], left, right, prevProofSize);
}

// WAR: do common context of 2 last proof
void mergeModifiedLemms() {
    cerr << "curPer" << curPermut << endl;
    int lst = proof.size() - 2;
    int numOfVar = (int) (log(curPermut - lst + 1) / log(2));

    cerr << "log():" << curPermut - lst + 1 << endl;
    cerr << "var.size():" << variables.size() << endl;
    string lastVarFromContext = variables[variables.size() - numOfVar];
    cerr << "lastVarFromContext:" << lastVarFromContext << endl;
    cerr << "resultProofExpr" << resultProofExpr << endl;

    proof[lst].insert(proof[lst].end(), proof.back().begin(), proof.back().end());
    proof.pop_back();

    int offset = proof[lst].size();
    proof[lst].resize(proof[lst].size() + aOrNota.size());

    modifiedStrings(aOrNota, &proof[lst], lastVarFromContext, "q", offset);
    
    proof[lst].push_back("(" + lastVarFromContext + "->" + resultProofExpr + ")->" +
                         "(!" + lastVarFromContext + "->" + resultProofExpr + ")->" +
                         "(" + lastVarFromContext + "|!" + lastVarFromContext + ")->" +
                         resultProofExpr);    

    proof[lst].push_back("(!" + lastVarFromContext + "->" + resultProofExpr + ")->" +
                         "(" + lastVarFromContext + " |!" + lastVarFromContext + ")->" +
                         resultProofExpr);    

    proof[lst].push_back("(" + lastVarFromContext + " |!" + lastVarFromContext + ")->" +
                         resultProofExpr);    

    proof[lst].push_back(resultProofExpr);    
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
        setModifiedLemms('!', leftVal, cur->left->exprSubTree, "q");
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
            //badAnswer[variables[j]] = curPermut & (1 << j);
            badAnswer[variables[j]] = curPermut == 0 ? 1 : 0;

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
        //cerr << "exprSubTree" << cur->exprSubTree << endl;
        curContext += cur->exprSubTree;
        proof.push_back(vector<string>(1));
        proof.back()[0] = curContext; 

        //cerr << "proof.size()" << proof.size() << endl;
        f = getValue(cur);
        if (!f)
            return false;


        main_deduction.doDeduction(proof.back());
        //cerr << "end" << endl;
        proof.back() = main_deduction.result; 

        resultProofExpr = main_deduction.resultProofExpr;

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
