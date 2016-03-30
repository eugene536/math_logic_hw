#include <cassert>
#include "tree.h"

namespace {
    inline std::vector<int> getPows(int maxLen, int x, int M) {
        std::vector<int> res(maxLen);
        res[0] = 1;
        for (int i = 1; i < maxLen; ++i)
            res[i] = static_cast<int>((res[i - 1] * 1LL * x) % M);
        return res;
    }
}

std::vector<int> Tree::pows_ = getPows(Tree::kMaxLen, Tree::kX, Tree::kM);

// func
Tree::Tree(const std::string &tag, const std::vector<Tree *> children)
    : tag_(tag)
    , children_(children)
{
    init();
}

// mult
Tree::Tree(const std::vector<char>& tag, Tree* child)
    : tag_(tag.begin(), tag.end())
    , children_{child}
{
    init();
}

// expr
Tree::Tree(const std::string& tag, Tree* left, boost::optional<Tree*> right) 
    : tag_(tag)
{
    assert(right);

    children_.push_back(left);
    children_.push_back(*right);

    init();
}

// pred
Tree::Tree(const char c, const std::vector<char> &tag, boost::optional<std::vector<Tree *> > opt)
    : tag_(c + std::string(tag.begin(), tag.end()))
{
    if (opt)
        children_ = *opt;

    init();
}

std::ostream& operator<<(std::ostream& out, const Tree* tree) {
    out << "|" << tree->tag_ << "|";
    for (Tree* child: tree->children_)
        out << ", " << child->tag_;
    return out;
}

void Tree::print(std::ostream& out, int depth) const {
    for (int i = 0; i < depth * 2; ++i)
        out << " ";

    out << tag_ << "\n";
    for (Tree* child: children_) {
        child->print(out, depth + 1); 
    }
}

bool Tree::operator==(const Tree &oth) const
{
    if (hash_ != oth.hash_)
        return false;
    return equals(this, &oth);
}

bool Tree::operator!=(const Tree &oth) const
{
    return !(*this == oth);
}

void Tree::init()
{
    hash_ = 0;
    len_ = 0;
    calculateHash();
}

void Tree::calculateHash()
{
    int len = static_cast<int>(tag_.size());

    hash_ = 0;
    for (char c: tag_)
        hash_ = (hash_ + c * kX) % kM;

    for (Tree* child: children_) {
        hash_ = static_cast<int>(((child->hash_ * 1LL * pows_[len]) % kM + hash_) % kM);
        len += child->len_;
    }
    len_ = len;
}

bool Tree::equals(const Tree *l, const Tree *r)
{
    assert(l && r);
    if (l->hash_ != r->hash_  ||
        l->len_ != r->len_ ||
        l->children_.size() != r->children_.size() ||
        l->tag_ != r->tag_)
    {
        return false;
    }

    for (size_t i = 0; i < l->children_.size(); ++i)
        if (!equals(l->children_[i], r->children_[i]))
            return false;

    return true;
}

