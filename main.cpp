//INV: data are correct
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

struct Tree {
    string str;
    long long hash;
    int size;
    Tree* left;
    Tree* right;
    Tree() {
        left = right = NULL;
        hash = size = 0;
    }
};
typedef Tree* linkOnTree;

enum Lexems {
    Begin,
    OpenBracket,
    CloseBracket,
    And,
    Or,
    Not,
    Entailment, // ->
    Variable, // ('A'..'Z'){'0'..'9'}*
    End
};
Lexems curLexem = Begin;
string var;

string s;
vector<linkOnTree> forest;
int it; //iterator

const int maxLen = 1e4;
long long q = 3;
long long qPow[maxLen];

void printTree(linkOnTree x, string s = "") {
    if (x == NULL) 
        return;
    printTree(x->left, s + "l");
    printTree(x->right, s + "r");
    cout << s << " : "<< x->str << endl;
}

bool good(int x) {
    return x >= 0 && x < (int) s.length();
}

bool goodCharForVar(char x) {
    return (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9');
}

char nextToken() {
    do {
       it++; 
    } while(good(it) && isspace(s[it]));
    return s[it];
}

void nextLexem() {
    nextToken();
    //cout << it << "  " << s[it] << endl;
    if (!good(it)) {
        curLexem = End;
        return;
    }

    switch(s[it]) {
        case '(':
            curLexem = OpenBracket;        
        break;

        case ')':
            curLexem = CloseBracket;        
        break;

        case '!':
            curLexem = Not;
        break;

        case '&':
            curLexem = And;
        break;

        case '|':
            curLexem = Or;
        break;

        case '-': // ->
            it++;
            if (good(it) && s[it] == '>') {
                curLexem = Entailment;        
            } else {
                throw runtime_error("unexpected token on position" + to_string(it));
            }
        break;

        default:
            if (s[it] >= 'A' && s[it] <= 'Z') {
                var = "";
                while (good(it) && goodCharForVar(s[it])) {
                   var.push_back(s[it]); 
                   it++;
                } 
                it--;
                curLexem = Variable;        
            } else {
                throw runtime_error("unexpected token on position" + to_string(it));
            }
        break;
    } 
}

long long getHashStr(string s) {
    long long temp = 0;
    for (int i = 0; i < (int) s.length(); i++) {
        temp += qPow[i] * s[i];
    }
    return temp;
}

linkOnTree expr();
linkOnTree disjunction();
linkOnTree conjunction();

linkOnTree updateVertex(linkOnTree vertex, linkOnTree left, linkOnTree right, string str) {
    if (!vertex) 
        return left;
    vertex->right = right;
    vertex->left = left;
    vertex->str = str;

    //cout << "in upd" << left->str << endl;
    int rsize = (vertex->right) ? vertex->right->size : 0; 
    int lsize = (vertex->left) ? vertex->left->size : 0; 

    int rhash = (vertex->right) ? vertex->right->hash : 0; 
    int lhash = (vertex->left) ? vertex->left->hash : 0; 

    vertex->size = lsize + rsize + 1;
    vertex->hash = lhash + qPow[lsize] * (getHashStr(str) + rhash * q);
    return vertex;
}

// not
linkOnTree nigation() { 
    //cout << "nig" << curLexem << endl;
    linkOnTree vertex = NULL;
    if (curLexem == Not) {
        vertex = new Tree;
        nextLexem();
        //cout << "upd" << curLexem << endl;
        updateVertex(vertex, nigation(), NULL, "!");
    } else if(curLexem == Variable) {
        vertex = new Tree;
        nextLexem();
        updateVertex(vertex, NULL, NULL, var); 
    } else if( curLexem == OpenBracket) {
        nextLexem();
        vertex = expr();
        //cerr << "-------" << endl;
        //printTree(vertex);
        //cerr << "-------" << endl;

        if (curLexem != CloseBracket) {
            throw runtime_error("expected ) on position" + to_string(it));    
        } else {
            nextLexem();
        }
    }
    return vertex;
}

// &
linkOnTree conjunction() {
    linkOnTree left = nigation();
    linkOnTree vertex = NULL;
    linkOnTree right = NULL;
    if (curLexem == And) {
        vertex = new Tree;
        nextLexem();
        right = conjunction();
    }
    return updateVertex(vertex, left, right, "&");
}

// |
linkOnTree disjunction() {
    linkOnTree left = conjunction();
    linkOnTree vertex = NULL;
    linkOnTree right = NULL;
    if (curLexem == Or) {
        vertex = new Tree;
        nextLexem();
        right = disjunction(); 
    }
    return updateVertex(vertex, left, right, "|");
}

// ->, (expr)
linkOnTree expr() {
    //cout << "lex in expr" << curLexem << endl;
    linkOnTree left = disjunction(); 
    linkOnTree right = NULL;
    linkOnTree vertex = NULL;
    if (curLexem == Entailment) {
        vertex = new Tree;
        nextLexem();
        right = expr();
    }
    return updateVertex(vertex, left, right, "->");
}

linkOnTree parse() {
    it = -1; 
    nextLexem(); 
    return expr();
}


int main() {
    #ifdef DEBUG
    freopen("in", "r", stdin);
    //freopen("out", "w", stdout);
    #endif

    qPow[0] = 1;
    for (int i = 1; i < maxLen; i++) {
        qPow[i] = qPow[i - 1] * q;
    }

    while (getline(cin, s)) {
        forest.push_back(parse());
    }

    printTree(forest[0]);
    printTree(forest[1]);
    cout << endl << (forest[0]->hash == forest[1]->hash) << endl;
    //getline(cin, s);
    //cout << "-----" << endl;
    //cout << s << endl;
    //cout << "-----" << endl;

    //for (auto it : data) {
        //cout << it << endl;
    //}
    return 0;
}

