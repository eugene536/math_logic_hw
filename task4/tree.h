#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <boost/optional.hpp>

class Tree {
public:
    Tree(const std::string& tag, const std::vector<Tree*> children);

    Tree(const std::string& tag, const std::string& lc, Tree* r);

    Tree(const std::vector<char>& tag, Tree* child);

    Tree(const char c, const std::vector<char>& tag, boost::optional<std::vector<Tree*>> opt);

    template<typename... T>
    Tree(const std::string &tag, T... child)
        : tag_(tag)
        , children_ {static_cast<Tree*>(child)...}
    {}


//private:
    long long hash_;
    std::string tag_;
    std::vector<Tree*> children_;
};
