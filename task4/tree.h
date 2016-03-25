#pragma once
#include <string>
#include <vector>
#include <algorithm>

class Tree {
public:
    Tree(const std::string& tag)
        : tag_(tag)
    {}

    Tree(const std::string& tag, const std::vector<Tree*> children)
        : tag_(tag)
        , children_(children)
    {}

private:
    long long hash_;
    std::string tag_;
    std::vector<Tree*> children_;
};
