#ifndef DEDUCTION_H
#define DEDUCTION_H
#include <vector>
#include <fstream>
#include <iostream>
#include "parser.h" // string, un_map
#define r_sz result.size() - 1

struct deduction {
    deduction();
    std::vector<std::string> result;
    void doDeduction(std::vector<std::string>&);
    int sz();
    std::string resultProofExpr;
    ~deduction();

private:
    std::string s;
    std::string lastContext;
    std::vector<parser::linkOnTree> forest;
    std::vector<parser::linkOnTree> axioms;
    std::unordered_map<std::string, long long> axiomToHash;
    int curExpr;
    int curAxiom;
    bool f;
    parser main_parser;

    std::string trimWhiteSpace(std::string const&); 
    bool good(int); 
    bool isContext();
    void output(std::string const&, std::string const&, std::string const& s3);

    void itIsAxiom(parser::linkOnTree, parser::linkOnTree);
    int isAxiom(parser::linkOnTree);
};
#endif

