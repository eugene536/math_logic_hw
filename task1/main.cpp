#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include "parser.h"

using namespace std;

string var;

string s;
vector<parser::linkOnTree> forest;
vector<parser::linkOnTree> axioms;
unordered_map<string, long long> axiomToHash;
int it; 

bool f;

string trimWhiteSpace(string const& s) {
    string temp;
    for (auto c : s) {
        if (!isspace(c)) {
            temp.push_back(c);
        }
    }
    return temp;
}

void output(int n, string const& s, int flag, int x = 0, int y = 0) {
    cout << "(" << n + 1 << ") " << trimWhiteSpace(s); 
    if (flag == 0)
        cout << " (Сх. акс. " << x << ")";
    else if (flag == 1)
        cout << " (M.P. " << x + 1 << ", " << y + 1 << ")";
    else
        cout << " (Не доказано)";
    cout << endl;
}


void itIsAxiom(parser::linkOnTree vertex, parser::linkOnTree axiom) {
    if (!axiom || !vertex) {
        if (axiom || vertex) 
            f = false;
        return;
    }

    if (goodCharForVar(axiom->str[0])) {
        if (axiomToHash.count(axiom->str)) {
            if (axiomToHash[axiom->str] != vertex->hash) {
                f = false;
            }
        } else {
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

int isAxiom(parser::linkOnTree vertex) {
    for (int i = 0; i < (int) axioms.size(); i++)  {
        axiomToHash.clear();
        f = true;
        itIsAxiom(vertex, axioms[i]);
        if (f) {
            return i + 1;
        }
    }
    return 0;
}


parser main_parser;
int main() {
    #ifdef DEBUG
    freopen("in", "r", stdin);
    freopen("out", "w", stdout);
    #endif

    axioms.push_back(main_parser.parse("A -> B -> A"));
    axioms.push_back(main_parser.parse("(A -> B) -> (A -> B -> C) -> (A -> C)"));
    axioms.push_back(main_parser.parse("A -> B -> A & B"));
    axioms.push_back(main_parser.parse("A & B -> A"));
    axioms.push_back(main_parser.parse("A & B -> B"));
    axioms.push_back(main_parser.parse("A -> A | B"));
    axioms.push_back(main_parser.parse("B -> A | B"));
    axioms.push_back(main_parser.parse("(A -> C) -> (B -> C) -> (A | B -> C)"));
    axioms.push_back(main_parser.parse("(A -> B) -> (A -> !B) -> !A"));
    axioms.push_back(main_parser.parse("!!A -> A"));


    int i = 0;
    while (getline(cin, s)) {
        if (!s.length())
            continue;
        forest.push_back(main_parser.parse(s));
        int nAxiom = isAxiom(forest[i]);
        if (nAxiom) {
            output(i, s, 0, nAxiom);
        } else {
            long long curHash = forest[i]->hash;
            long long leftNewHash = 0;
            bool flag = false;
            for (int j = i - 1; j >= 0 && !flag; j--) {
                if (forest[j]->right && forest[j]->right->hash == curHash) {
                    leftNewHash = forest[j]->left->hash;
                    for (int z = i - 1; z >= 0; z--) {
                        if (forest[z]->hash == leftNewHash) {
                            flag = true;
                            output(i, s, 1, z, j);
                            break;
                        }
                    }
                }
            }
            if (!flag) {
                output(i, s, 2);
                return 0;
            }
        }
        i++;
    }

    return 0;
}
