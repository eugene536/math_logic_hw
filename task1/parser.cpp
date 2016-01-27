#include <iostream>
#include "parser.h"
using namespace std;

parser::parser() {
    q = 3;
    parser::qPow[0] = 1;
    for (int i = 1; i < maxLen; i++) {
        qPow[i] = qPow[i - 1] * q;
    }
}

parser::Tree::Tree(){
    left = right = NULL;
    hash = size = 0;
}

parser::Tree::~Tree() {
    delete left;
    delete right;
}

void parser::print(parser::linkOnTree t) {
    if (t == NULL) 
        return;
    print(t->left);
    print(t->right);
    std::cerr << t->str << std::endl;
}

bool parser::good(int x) {
    return x >= 0 && x < (int) s.length();
}

bool goodCharForVar(char x) {
    return (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9');
}

char parser::nextToken() {
    do {
       it++; 
    } while(good(it) && isspace(s[it]));
    return s[it];
}

void parser::nextLexem() {
    nextToken();
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
                //cerr << s << endl;
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
                //cerr << s << endl;
                throw runtime_error("unexpected token on position" + to_string(it));
            }
        break;
    } 
}

long long parser::getHashStr(string const& s) {
    long long temp = 0;
    for (int i = 0; i < (int) s.length(); i++) {
        temp += qPow[i] * s[i];
    }
    return temp;
}

parser::linkOnTree parser::updateVertex(linkOnTree vertex, linkOnTree left, linkOnTree right, string const& str) {
    if (!vertex) 
        return left;
    vertex->right = right;
    vertex->left = left;
    vertex->str = str;

    int rsize = (vertex->right) ? vertex->right->size : 0; 
    int lsize = (vertex->left) ? vertex->left->size : 0; 

    int rhash = (vertex->right) ? vertex->right->hash : 0; 
    int lhash = (vertex->left) ? vertex->left->hash : 0; 

    // new hash = hash( `(` + ltree + `)` + str + `(` + rtree + `)` )
    vertex->hash = 0;
    size_t cur_len = 0;
    if (lsize) {
        vertex->hash = '(' + q * lhash + ')' * qPow[lsize + 1]; // `(` + ltree + `)`
        cur_len = lsize + 2;
    }

    vertex->hash += getHashStr(str) * qPow[cur_len]; // str + `(`
    cur_len += str.length();

    if (rsize) {
        vertex->hash += ('(' + rhash * q + ')' * qPow[rsize + 1]) * qPow[cur_len];
        cur_len += rsize + 2;
    }
    vertex->size = cur_len;

    string lSubStr = (vertex->left) ? "(" + vertex->left->exprSubTree + ")" : "";
    string rSubStr = (vertex->right) ?  "(" + vertex->right->exprSubTree + ")" : "";
    vertex->exprSubTree = lSubStr + str + rSubStr;
    return vertex;
}

// not
parser::linkOnTree parser::nigation() { 
    linkOnTree vertex = NULL;
    if (curLexem == Not) {
        vertex = new Tree;
        nextLexem();
        updateVertex(vertex, NULL, nigation(), "!");
    } else if(curLexem == Variable) {
        vertex = new Tree;
        nextLexem();
        updateVertex(vertex, NULL, NULL, var); 
    } else if( curLexem == OpenBracket) {
        nextLexem();
        vertex = expr();

        if (curLexem != CloseBracket) {
            throw runtime_error("expected ) on position" + to_string(it) + "\n" + s);    
        } else {
            nextLexem();
        }
    }
    return vertex;
}

// &
parser::linkOnTree parser::conjunction() {
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
parser::linkOnTree parser::disjunction() {
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
parser::linkOnTree parser::expr() {
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

parser::linkOnTree parser::parse(const string& s2) {
    curLexem = Begin;
    s = s2;
    it = -1; 
    nextLexem(); 
    return expr();
}


namespace {
    bool equals(const parser::Tree* fst, const parser::Tree* scd) {
        if (!fst && !scd) {
            return true;
        } if (!fst || !scd) {
            return false;
        }

        if (fst->str != scd->str || fst->hash != scd->hash) return false;
        return equals(fst->left, scd->left) && equals(fst->right, scd->right);
    }
}

bool operator==(const parser::Tree& fst, const parser::Tree& scd) 
{
    if (&fst == &scd) return true;
    return equals(&fst, &scd);
}

bool operator!=(const parser::Tree& fst, const parser::Tree& scd) {
   return !(fst == scd);
}
