#include <iostream>
#include <fstream>
#include <cctype>
#include <numeric>
#include <utility>
#include <algorithm>

#include "kripke.h"
#include "parser.h"

Kripke::Kripke(const std::string &path)
{
    std::string str;
    std::ifstream(kPathResources + "/" + path) >> str;
    expr_ = parse(str);

    std::unordered_set<std::string> vars;
    getVars(expr_, vars);
    for (const std::string& s: vars)
        vars_.push_back(s);
}

void Kripke::start(const std::string &out_path)
{
    out_.open(out_path);
    const int kCntIter = 7;
    for (int i = 1; i <= kCntIter; ++i) {
        parent_.resize(i, -1);
        edges_.resize(i);
        masks_.resize(i, 0);
        generateTree(1);
    }
}

void Kripke::getVars(Tree *expr, std::unordered_set<std::string> &vars)
{
    if (std::isupper(expr->tag_[0])) {
        vars.insert(expr->tag_);
        return;
    }
    for (Tree* child: expr->children_)
        getVars(child, vars);
}

bool Kripke::calcExpr(Tree *expr, int world)
{
    std::vector<Tree*>& childs = expr->children_;

    switch (expr->tag_[0]) {
        case '&':
            return calcExpr(childs[0], world) && calcExpr(childs[1], world);
        case '|':
            return calcExpr(childs[0], world) || calcExpr(childs[1], world);
        case '-':
            return checkImplication(childs[0], childs[1], world);
        case '!':
            return checkNot(childs[0], world);
        default: // var
            int pos = std::find(vars_.begin(), vars_.end(), expr->tag_) - vars_.begin();
            return masks_[world] & (1 << pos);
    }
}

bool Kripke::checkNot(Tree *expr, int world)
{
    if (calcExpr(expr, world))
        return 0;

    for (int w: edges_[world])
        if (!checkNot(expr, w))
            return false;

    return true;
}

bool Kripke::checkImplication(Tree *l, Tree *r, int world)
{
    if (calcExpr(l, world) && !calcExpr(r, world))
        return 0;

    for (int w: edges_[world])
        if (!checkImplication(l, r, w))
            return false;
    return true;
}

void Kripke::generateWorlds(int cur)
{
    if (cur == (int) parent_.size()) {
        if (!calcExpr(expr_, 0)) {
            std::cerr << "fail" << std::endl;
            for (size_t i = 0; i < edges_.size(); i++) {
                if (!edges_[i].empty()) {
                    out_ << i << ": ";
                    for (int e: edges_[i])
                        out_ << e << " ";
                    out_ << "\n";
                }

                if (masks_[i] != 0) {
                    out_ << i << ": ";
                    for (size_t j = 0; j < vars_.size(); j++)
                        if (masks_[i] & (1 << j))
                            out_ << vars_[j] << " ";
                    out_ << "\n";
                }
            }
            out_.flush();
            exit(0);
        }
        return;
    }

    int mask = (parent_[cur] == -1) ? 0: masks_[parent_[cur]];

    for (int mm = 0; mm < (1 << vars_.size()); ++mm) {
        if ((~mm & mask) == 0) {
            masks_[cur] = mm;
            generateWorlds(cur + 1);
        }
    }
}

void Kripke::generateTree(int cur)
{
    if (cur == (int) parent_.size()) {
        for (size_t i = 0; i < edges_.size(); ++i)
            edges_[i].clear();

        for (int i = 1; i < (int) parent_.size(); ++i)
            edges_[parent_[i]].push_back(i);

        generateWorlds(0);

        return;
    }

    for (int i = 0; i < cur; ++i) {
        parent_[cur] = i;
        generateTree(cur + 1);
    }
}
