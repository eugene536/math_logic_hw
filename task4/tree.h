#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/optional.hpp>

class Tree {
public:
    Tree(const std::string& tag, const std::vector<Tree*> children);

    Tree(const std::string& tag, const std::string& lc, Tree* r);

    Tree(const std::vector<char>& tag, Tree* child);

    Tree(const std::string& tag, Tree* left, boost::optional<Tree*> right);

    Tree(const char c, const std::vector<char>& tag, boost::optional<std::vector<Tree*>> opt);

    template<typename... T>
    Tree(const std::string &tag, T... child)
        : tag_(tag)
        , children_ {static_cast<Tree*>(child)...}
    {}

    void print(std::ostream& out, int depth = 0) const;

private:
    long long hash_;
    std::string tag_;
    std::vector<Tree*> children_;

private:
    friend std::ostream& operator<<(std::ostream& out, const Tree* tree);
};
