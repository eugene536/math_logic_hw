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

bool deduction::itIsAxiom(parser::linkOnTree vertex, parser::linkOnTree axiom) {
    if (!axiom && !vertex) {
        return true;
    } else if (!axiom || !vertex) {
        return false;
    } 

    if (goodCharForVar(axiom->str[0])) {
        if (isContext()) {
            return axiom->str == vertex->str;
        } else if (axiomToHash.count(axiom->str)) {
            return *axiomToHash[axiom->str] == *vertex;
        } else {
            axiomToHash[axiom->str] = vertex;
            return true;
        }
    } else if (axiom->str == vertex->str) {
        return itIsAxiom(vertex->left, axiom->left) && 
               itIsAxiom(vertex->right, axiom->right); 
    }

    return false;
}

int deduction::isAxiom(parser::linkOnTree vertex) {
    for (curAxiom = 0; curAxiom < (int) axioms.size(); curAxiom++)  {
        axiomToHash.clear();
        if (itIsAxiom(vertex, axioms[curAxiom])) {
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
        //cerr << curExpr - 1 << ": " << s;

        history.push_back(s);
        forest.push_back(main_parser.parse(s));

        int nAxiom = isAxiom(forest[i]);
        //cerr << nAxiom << endl;
        if (nAxiom) {
            //cerr << "axiom: " << result.size() << endl;
            result.push_back(s);
            result.push_back(s + "->" + lastContext + "->" + s);
            result.push_back(lastContext + "->" + s);
            //cerr << "axiom: " << result.size() << endl;
        } else { // MP
            //cerr << "str--" << s << "---str" << endl;
            //cerr << i << "startDoDed1 : " << forest[i] << "  " << forest.size() << endl;

            const parser::Tree& lastExpr = *forest[i];

            //cerr << "startDoDed2" << endl;
            bool flag = false;
            for (int j = i - 1; j >= 0 && !flag; j--) {
                if (forest[j]->right && forest[j]->str == "->" && *forest[j]->right == lastExpr) {
                    const parser::Tree& leftNewExpr = *forest[j]->left;
                    for (int z = i - 1; z >= 0; z--) {
                            if (*forest[z] == leftNewExpr) {
                                flag = true;
                            //cerr << "MP: " << result.size() << " pos: " << i << "; from: " << j << "; to: " << z << endl;
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
                //cerr << "a->a: " << " " << result.size() << endl;
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
