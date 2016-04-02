#ifndef DEDUCTION_H
#define DEDUCTION_H
#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>

#include "tree.h"

class Kripke {
public:
    Kripke(const std::string& path);

    void start(const std::string& out_path);

//private:
    const std::string kPathResources = "resources";

//private:
    static void getVars(Tree* expr, std::unordered_set<std::string> &vars);

    bool calcExpr(Tree* expr, int world);

    bool checkNot(Tree* expr, int world);

    bool checkImplication(Tree* l, Tree* r, int world);

    void generateWorlds(int cur);

    void generateTree(int cur);

private:
    Tree* expr_;
    std::ofstream out_;
    std::vector<int> parent_;
    std::vector<std::vector<int>> edges_;
    std::vector<std::string> vars_;
    std::vector<int> masks_;
};

#endif // DEDUCTION_H
