#include <cassert>
#include "tree.h"

Tree::Tree(const std::string &tag, const std::vector<Tree *> children)
    : tag_(tag)
    , children_(children) {
}

Tree::Tree(const std::string &tag, const std::string &lc, Tree *r)
    : tag_(tag)
    , children_{new Tree(lc), r}
{}

Tree::Tree(const std::vector<char>& tag, Tree* child)
    : tag_(tag.begin(), tag.end())
    , children_{child}
{}

Tree::Tree(const std::string& tag, Tree* left, boost::optional<Tree*> right) 
    : tag_(tag)
{
    assert(right);

    children_.push_back(left);
    children_.push_back(*right);
}

Tree::Tree(const char c, const std::vector<char> &tag, boost::optional<std::vector<Tree *> > opt)
{
    tag_ = c + std::string(tag.begin(), tag.end());
    if (opt)
        children_ = *opt;
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


//#include <iostream>
//#include <string>
//#include <boost/phoenix/core.hpp>
//#include <boost/phoenix/operator.hpp>
//#include <boost/phoenix/object/new.hpp>
//using namespace boost::phoenix;
//using namespace boost::phoenix::arg_names;

//int main() {
//    new Tree("asdf");
//    std::cerr << "work" << std::endl;
//    std::cerr << ((arg1 * arg2)(20, 30)) << std::endl;
//    Tree* p = boost::phoenix::new_<Tree>("hello_pointer")();
//    std::cerr << p << std::endl;
//    std::string *s = new std::string("hello");
//    std::cerr << *s << std::endl;

//    Tree *t1 = new Tree("child");
//    Tree *tree = new Tree("name", t1);
//    std::cerr << tree->tag_ << " " << tree->children_.size() << std::endl;
//    std::cerr << tree->children_[0]->tag_ << std::endl;
//    return 0;
//}


