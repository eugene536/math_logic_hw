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
    Tree(string s = "") {
        left = right = NULL;
        hash = size = 0;
        str = s;
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
const char arrow = 19;
long long q = 3;
long long qPow[maxLen];

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
    cout << it << "  " << s[it] << endl;
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
                curLexem = OpenBracket;        
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

// not
linkOnTree nigation() { 
    linkOnTree vertex = NULL;
    if (curLexem == Not) {
        vertex = new Tree();
        vertex->str = "!";
        nextLexem();
        vertex->left = nigation();
        int rsize = (vertex->right) ? vertex->right->size : 0; 
        int lsize = (vertex->left) ? vertex->left->size : 0; 

        int rhash = (vertex->right) ? vertex->right->hash : 0; 
        int lhash = (vertex->left) ? vertex->left->hash : 0; 

        vertex->size = lsize + rsize + 1;
        vertex->hash = lhash + qPow[lsize] * ('!' + rhash * q);
    } else if(curLexem == Variable) {
        vertex = new Tree();
        vertex->str = var;
        vertex->size = 1;
        vertex->hash = getHashStr(var);
    } else if( curLexem == OpenBracket) {
        nextLexem();
        vertex = expr();

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
    linkOnTree left = conjunction();
    linkOnTree vertex = NULL;
    if (curLexem == Or) {
        vertex = new Tree("&");
        vertex->left = left;
        nextLexem();
        vertex->right = disjunction();

        int rsize = vertex->right->size;
        int lsize = vertex->left->size;

        int rhash = vertex->right->hash; 
        int lhash = vertex->left->hash; 

        vertex->size = lsize + rsize + 1;
        vertex->hash = lhash + qPow[lsize] * ('&' + rhash * q);

    }
    if (!vertex) 
        return left;
    return vertex;
}

// |
linkOnTree disjunction() {
    linkOnTree left = conjunction();
    linkOnTree vertex = NULL;
    if (curLexem == Or) {
        vertex = new Tree("|");
        vertex->left = left;
        nextLexem();
        vertex->right = disjunction();

        int rsize = vertex->right->size;
        int lsize = vertex->left->size;

        int rhash = vertex->right->hash; 
        int lhash = vertex->left->hash; 

        vertex->size = lsize + rsize + 1;
        vertex->hash = lhash + qPow[lsize] * ('|' + rhash * q);

    }
    if (!vertex) 
        return left;
    return vertex;
}

// ->, (expr)
linkOnTree expr() {
    linkOnTree left = disjunction(); 
    linkOnTree vertex = NULL;
    if (curLexem == Entailment) {
        linkOnTree vertex = new Tree(string(1, arrow));
        vertex->left = left;
        nextLexem();
        vertex->right = expr();

        int rsize = vertex->right->size;
        int lsize = vertex->left->size;

        int rhash = vertex->right->hash; 
        int lhash = vertex->left->hash; 

        vertex->size = lsize + rsize + 1;
        vertex->hash = lhash + qPow[lsize] * (arrow + rhash * q);
    }
    if (!vertex) 
        return left;
    return vertex;
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
    //getline(cin, s);
    //cout << "-----" << endl;
    //cout << s << endl;
    //cout << "-----" << endl;

    //for (auto it : data) {
        //cout << it << endl;
    //}
    return 0;
}

