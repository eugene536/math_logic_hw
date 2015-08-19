#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <unordered_map>

struct parser {
    parser();
    struct Tree {
        std::string str;
        long long hash;
        int size;
        Tree* left;
        Tree* right;
        Tree();
        ~Tree();
    };

    typedef Tree* linkOnTree;
    linkOnTree parse(const std::string& s2);
    void print(linkOnTree);

private:
    enum Lexems {
        Begin, OpenBracket, CloseBracket, And, Or, Not, Entailment, Variable, End
    };

    Lexems curLexem;
    std::string var;
    std::string expression;
    std::unordered_map<std::string, long long> axiomToHash;
    static const int maxLen = 1e4;
    long long qPow[maxLen];
    int it; 

    long long q = 3;

    bool good(int); 
    char nextToken();
    void nextLexem();
    long long getHashStr(std::string const&);

    linkOnTree updateVertex(linkOnTree, linkOnTree, linkOnTree, std::string const&);
    linkOnTree nigation();
    linkOnTree conjunction();
    linkOnTree disjunction();
    linkOnTree expr();

    friend bool operator==(const Tree& fst, const Tree& scd);
};
bool goodCharForVar(char);

bool operator==(const parser::Tree& fst, const parser::Tree& scd);
#endif
