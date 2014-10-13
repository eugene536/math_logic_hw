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
    std::string s;
    std::unordered_map<std::string, long long> axiomToHash;
    static const int maxLen = 1e4;
    long long qPow[maxLen];
    int it; 

    long long q;

    bool good(int const&); 
    char nextToken();
    void nextLexem();
    long long getHashStr(std::string const&);

    linkOnTree updateVertex(linkOnTree, linkOnTree, linkOnTree, std::string const&);
    linkOnTree nigation();
    linkOnTree conjunction();
    linkOnTree disjunction();
    linkOnTree expr();
};
bool goodCharForVar(char const&);
#endif
