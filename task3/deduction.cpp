#include "deduction.h"
using namespace std;

bool deduction::good(int x) {
    return x >= 0 && x < (int) s.length();
}

string deduction::trimWhiteSpace(string const& s) {
    string temp;
    for (auto c : s)
        if (!isspace(c))
            temp.push_back(c);
    return temp;
}

bool deduction::isContext() {
    return curAxiom >= 10;
}

void deduction::itIsAxiom(parser::linkOnTree vertex, parser::linkOnTree axiom) {
    if (!axiom || !vertex) {
        if (axiom || vertex) {
            f = false;
        } 
        return;
    }

    if (goodCharForVar(axiom->str[0])) {
        if (isContext() || axiomToHash.count(axiom->str)) {
            if ((!isContext() && (axiomToHash[axiom->str] != vertex->hash)) ||
                 (isContext() && (axiom->str != vertex->str))) {
                f = false;
            }
        } else if (!isContext()){
            axiomToHash[axiom->str] = vertex->hash;
        }
    } else if (axiom->str == vertex->str) {
        itIsAxiom(vertex->left, axiom->left); 
        itIsAxiom(vertex->right, axiom->right); 
    } else {
        f = false;
        return;
    }
}

int deduction::isAxiom(parser::linkOnTree vertex) {
    for (curAxiom = 0; curAxiom < (int) axioms.size(); curAxiom++)  {
        axiomToHash.clear();
        f = true;
        itIsAxiom(vertex, axioms[curAxiom]);
        if (f) {
            return curAxiom + 1;
        }
    }
    return 0;
}

void deduction::output(string const& s1, string const& s2, string const& s3 = "") {
    result[r_sz] = "(" + s1 + ")" + "->(" + s2 + ")";
    if (s3 != "") 
        result[r_sz] = "->(" + s1 + ")";
}

deduction::deduction() {
    ifstream in("axioms/base_axioms");
    if (!in)
        throw runtime_error("Can't open file axioms/base_axioms");
    string s;
    while(in >> s)
        axioms.push_back(main_parser.parse(s));
    in.close();
}

deduction::~deduction() {
    for (int i = 0; i < (int) axioms.size(); i++) {
        delete axioms[i];
    }

    for (int i = 0; i < (int) forest.size(); i++) {
        delete forest[i];
    }
}

//change const& on &
void deduction::doDeduction(vector<string>& expressions) { 
    vector<string> history;
    string exprAfterDeduction;
    s = "";
    curExpr = 0;
    result.clear();
    forest.clear();
    f = false;

    lastContext = expressions[curExpr++];
    int turniketPos = lastContext.find("|-");
    int commaPos = 0;
    int cntNewAxioms = 0;
    for (int i = 0; i < turniketPos; i++) {
        if (lastContext[i] == ',') {
            commaPos = i;
            axioms.push_back(main_parser.parse(s));
            cntNewAxioms++;
            s = "";
            continue;
        }
        s.push_back(lastContext[i]);
    }

    if (!s.length()) { // if expr is without context
        result = expressions;
        return;
    }

    exprAfterDeduction = lastContext.substr(0, commaPos);
    resultProofExpr = lastContext.substr(turniketPos + 2);
    if (exprAfterDeduction.size()) { // if Context != "" 
        exprAfterDeduction += "|-" + resultProofExpr;
        result.push_back(exprAfterDeduction); // .push_back(newContext without lastContext)
    }
    lastContext = "(" + s + ")";

    //cerr << expressions[0] << endl;
    for (int i = 1; i < (int) expressions.size(); i++) {
        expressions[i] = "(" + expressions[i] + ")";
    }

    int i = 0;
    //cerr << "size" << expressions.size() << endl;
    for (;curExpr < (int) expressions.size(); curExpr++) {
        s = expressions[curExpr];

        history.push_back(s);
        forest.push_back(main_parser.parse(s));

        int nAxiom = isAxiom(forest[i]);
        if (nAxiom) {
            result.push_back(s);
            result.push_back(s + "->" + lastContext + "->" + s);
            result.push_back(lastContext + "->" + s);
        } else { // MP
            //cerr << "str--" << s << "---str" << endl;
            //cerr << i << "startDoDed1 : " << forest[i] << "  " << forest.size() << endl;

            long long curHash = forest[i]->hash;

            //cerr << "startDoDed2" << endl;
            long long leftNewHash = 0;
            bool flag = false;
            for (int j = i - 1; j >= 0 && !flag; j--) {
                if (forest[j]->right && forest[j]->right->hash == curHash) {
                    leftNewHash = forest[j]->left->hash;
                    for (int z = i - 1; z >= 0; z--) {
                            if (forest[z]->hash == leftNewHash) {
                                flag = true;
                            result.push_back('(' + lastContext + "->" + history[z] + ")->");
                            result[r_sz] += "(" + lastContext + "->" + history[z] + "->" + s + ")->";
                            result[r_sz] += lastContext + "->" + s;

                            result.push_back("(" + lastContext + "->" + history[z] + "->" + s + ")->");
                            result[r_sz] += lastContext + "->" + s;

                            result.push_back(lastContext + "->" + s);
                            break;
                        }
                    }
                }
            }

            //!MP
            //a -> a
            if (!flag) {
                // a->a->a
                result.push_back(lastContext + "->" + lastContext + "->" + lastContext);

                //aks 2
                result.push_back("(" + lastContext + "->" + lastContext + "->" + lastContext + ")->");
                result[r_sz] += "(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")->";
                result[r_sz] += lastContext + "->" + lastContext;

                //MP
                result.push_back("(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")->");
                result[r_sz] += lastContext + "->" + lastContext;

                // aks 1
                result.push_back("(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")" );

                //MP
                result.push_back(lastContext + "->" + lastContext);
            }
        }
        i++;
    }

    for (int i = (int) axioms.size() - cntNewAxioms; i < (int) axioms.size(); i++) {
        delete axioms[i];
    }

    axioms.erase(axioms.end() - cntNewAxioms, axioms.end());
                        //cerr << "endDoDed" << endl;
}
