#ifndef DEDUCTION_H
#define DEDUCTION_H
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <boost/optional.hpp>

#include "tree.h"

class Deduction {
public:
    Deduction(const std::string& path);

    void doDeduction(const std::string& path);

//private:
    const std::string kPathResources = "resources";
    const std::string kPathAxioms = kPathResources + "/axioms/base_axioms";
    const std::string kPathFormalAxioms = kPathResources + "/axioms/base_formal_axioms";

//private:
    void init();

    void readAxioms();

    void readFormalAxioms();

    static boost::optional<std::pair<int, int>> isMP(Tree* expr, const std::vector<Tree*>& context);

    static boost::optional<int> isForallRule(Tree* expr, const std::vector<Tree*>& context);

    static boost::optional<int> isExistRule(Tree* expr, const std::vector<Tree*>& context);

    bool isAxiom(Tree* expr) const;

    static bool isAxiomHelper(Tree* expr, Tree *axiom, std::unordered_map<char, Tree *> &m);

    bool isFormalAxiom(Tree* expr) const;

    static bool isInduction(Tree* expr);

    static bool equalsWithSubstitution(Tree* orig, Tree* expr, const std::string& var,
                                       Tree* new_var, std::unordered_multiset<std::string> &bounded);

    static bool isAA(Tree* expr);

    static bool isForallAxiom(Tree* expr);

    static bool isExistAxiom(Tree* expr);

    static bool isFreeForSubstitution(Tree *orig, Tree* expr, const std::string& var,
                                      std::unordered_multiset<std::string> &bounded);

    static bool isFreeForSubstitutionHelper(Tree *orig, Tree* expr, const std::string& var,
                                            std::unordered_multiset<std::string> &bounded, Tree *&phi);

    static bool isVar(Tree* expr);

    static void getFreeVars(Tree* expr, std::unordered_set<std::string> &vars,
                            std::unordered_multiset<std::string>& bounded);

    // return true if var is free inside expr
    static bool isFree(Tree* expr, const std::string& var);

private:
    std::ifstream in_;
    std::vector<Tree*> axioms_;
    std::vector<Tree*> formal_axioms_;
};

#endif // DEDUCTION_H
