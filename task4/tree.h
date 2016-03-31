#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/optional.hpp>

class Deduction;

class Tree {
public:
    Tree(const std::string& tag, const std::vector<Tree*> children);

    Tree(const std::vector<char>& tag, Tree* child);

    Tree(const std::string& tag, Tree* left, boost::optional<Tree*> right);

    Tree(const char c, const std::vector<char>& tag, boost::optional<std::vector<Tree*>> opt);

    template<typename... T>
    Tree(const std::string &tag, T... child)
        : tag_(tag)
        , children_ {static_cast<Tree*>(child)...}
    {
        init();
    }

    void print(std::ostream& out, int depth = 0) const;

    bool operator==(const Tree& oth) const;

    bool operator!=(const Tree& oth) const;

    void calculateExpr();

private:
    void init();

    void calculateHash();

    static bool equals(const Tree* l, const Tree* r);

private:
    const static int kM = 1e9 + 7;
    const static int kX = 10123;
    const static int kMaxLen = 1e7;

private:
    static std::vector<int> pows_;
    int hash_;
    int len_;

    std::string expr_;
    std::string tag_;
    std::vector<Tree*> children_;

private:
    friend class Deduction;

    friend std::ostream& operator<<(std::ostream& out, const Tree* tree);
};
