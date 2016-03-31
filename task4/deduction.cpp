#include "deduction.h"
#include "parser.h"

#include <memory>
#include <sstream>
#include <tuple>

Deduction::Deduction(const std::string &path)
    : in_(kPathResources + "/" + path)
{
    init();
}

void Deduction::doDeduction(const std::string &out_path)
{
    using namespace std;

    string header;
    getline(in_, header);

    vector<Tree*> context;
    Tree* result;

    ofstream log_("log");
    std::tie(context, result) = parseHeader(header, log_);
    assert(result);

    ostringstream out;

    bool needWork = context.size() != 0;
    Tree* alpha = nullptr;
    unordered_set<string> vars;
    if (needWork) {
        alpha = context.back();
        context.pop_back();

        for (size_t i  = 0; i < context.size(); ++i)
            out << context[i] << (i == context.size() - 1 ? " " : ",");
        out << "|-" << alpha << "->" << result << endl;

        unordered_multiset<string> unused;
        getFreeVars(alpha, vars, unused);

    } else {
        out << "|-" << result << endl;
    }

#define notNeedWork() \
    if (!needWork) { \
        out << expr << endl; \
        cur_l++; \
        continue;\
    }

    string expr_s;
    size_t cur_l = 2;
    vector<Tree*> parsed;
    while (getline(in_, expr_s)) {
        Tree* expr = parse(expr_s);
        parsed.push_back(expr);
        assert(expr);

        if (alpha != nullptr && *expr == *alpha) {
            out << alpha << "->" << alpha << "->" << alpha << endl;

            out << "(" << alpha << "->" << alpha << "->" << alpha << ")->"
                << "(" << alpha << "->((" << alpha << "->" << alpha << ")->" << alpha << "))->"
                << "(" << alpha << "->" << alpha << ")" << endl;

            out << "(" << alpha << "->((" << alpha << "->" << alpha << ")->" << alpha << "))" << endl;

            out << "(" << alpha << "->((" << alpha << "->" << alpha << ")->" << alpha << "))->"
                << "(" << alpha << "->" << alpha << ")" << endl;

            out << "(" << alpha << "->" << alpha << ")" << endl;
        } else if (isAxiom(expr) ||
                   isForallAxiom(expr) ||
                   isExistAxiom(expr)  ||
                   isFormalAxiom(expr) ||
                   isFromContext(expr, context)) {
            notNeedWork();
            out << expr << endl;
            out << expr << "->" << alpha << "->" << expr << endl;
            out << alpha << "->" << expr << endl;
        } else if (isMP(expr, parsed)) {
            notNeedWork();
            int i, j;
            std::tie(i, j) = *isMP(expr, parsed);
            out << "(" << alpha << "->" << parsed[i] << ")->"
                   "(" << alpha << "->" << parsed[j] << ")->"
                   "(" << alpha << "->" << expr << ")" << endl;

            out << "(" << alpha << "->" << parsed[j] << ")->"
                   "(" << alpha << "->" << expr << ")" << endl;

            out << "(" << alpha << "->" << expr << ")" << endl;
        } else if (isForallRule(expr, parsed) && vars.count(expr->children_[1]->children_[0]->tag_) == 0) {
            notNeedWork();
//            int i = *isForallRule(expr, parsed);


        } else if (isExistRule(expr, parsed) && vars.count(expr->children_[0]->children_[0]->tag_) == 0) {
            notNeedWork();

        } else {
            log_ << "something go wrong on line: " << cur_l << endl;
            exit(EXIT_FAILURE);
        }

        cur_l++;
    }

    ofstream(out_path) << out.str();
}

void Deduction::init()
{
    readAxioms();
    readFormalAxioms();

    std::stringstream ss;
    ss << std::ifstream(kPathAbcBac).rdbuf();
    abc_bac = ss.str();

    ss.str("");
    ss << std::ifstream(kPathAandBC_abc).rdbuf();
    AandBC_abc = ss.str();

    ss.str("");
    ss << std::ifstream(kPathAbc_AandBC).rdbuf();
    abc_AandBC = ss.str();
}

void Deduction::readAxioms()
{
    std::ifstream file(kPathAxioms);
    std::string axiom;

    while (std::getline(file, axiom)) {
        if (axiom.empty()) continue;

        axioms_.push_back(parse(axiom));
        assert(axioms_.back());
    }

    std::cerr << "read axioms" << std::endl;
}

void Deduction::readFormalAxioms()
{
    std::ifstream file(kPathFormalAxioms);
    std::string axiom;

    while (std::getline(file, axiom)) {
        if (axiom.empty()) continue;

        formal_axioms_.push_back(parse(axiom));
        assert(formal_axioms_.back());
    }

    std::cerr << "read formal axioms" << std::endl;
}

bool Deduction::isFromContext(Tree *expr, const std::vector<Tree *> &context)
{
    for (Tree* t: context)
        if (*expr == *t)
            return true;
    return false;
}

boost::optional<std::pair<int, int>> Deduction::isMP(Tree* expr, const std::vector<Tree *> &context)
{
    for (size_t i = 0; i < context.size(); ++i) {
        Tree* c = context[i];

        if (c->tag_ == "->" && *c->children_[1] == *expr) {
            Tree* lc = c->children_[0];
            for (size_t j = 0; j < i; ++j) {
                if (*context[j] == *lc)
                    return std::pair<int, int>(j, i);
            }
        }
    }

    return boost::optional<std::pair<int, int>>();
}

boost::optional<int> Deduction::isForallRule(Tree *expr, const std::vector<Tree *> &context)
{
    // (phi) -> @x(psi), x is not free in phi
    if (expr->tag_ == "->") {
        Tree* phi = expr->children_[0]; // phi
        Tree* psi = expr->children_[1]; // @x(psi)
        if (psi->tag_ == "@") {
            std::string var = psi->children_[0]->tag_; // x
            psi = psi->children_[1]; // psi
            std::unordered_set<std::string> vars;
            std::unordered_multiset<std::string> unused;

            getFreeVars(phi, vars, unused);

            if (!vars.count(var)) { // var is not free in phi
                std::unique_ptr<Tree> form(new Tree("->", phi, psi));// form := phi->psi
                for (size_t i = 0; i < context.size(); ++i)
                    if (*context[i] == *form)
                        return boost::optional<int>(i);
            }
        }
    }

    return boost::optional<int>();
}

boost::optional<int> Deduction::isExistRule(Tree *expr, const std::vector<Tree *> &context)
{
    // ?x(psi) -> (phi), x is not free in phi
    if (expr->tag_ == "->") {
        Tree* psi = expr->children_[0]; // ?x(psi)
        Tree* phi = expr->children_[1]; // phi
        if (psi->tag_ == "?") {
            std::string var = psi->children_[0]->tag_; // x
            psi = psi->children_[1]; // psi
            std::unordered_set<std::string> vars;
            std::unordered_multiset<std::string> unused;

            getFreeVars(phi, vars, unused);

            if (!vars.count(var)) { // var is not free in phi
                std::unique_ptr<Tree> form(new Tree("->", psi, phi));// form := psi->phi
                for (size_t i = 0; i < context.size(); ++i)
                    if (*context[i] == *form)
                        return boost::optional<int>(i);
            }
        }
    }

    return boost::optional<int>();
}

bool Deduction::isAxiom(Tree *expr) const
{
    assert(expr);

    static std::unordered_map<char, Tree*> m;

    for (Tree* axiom: axioms_) {
        m.clear();
        if (isAxiomHelper(expr, axiom, m)) {
            return true;
        }
    }

    return false;
}

bool Deduction::isAxiomHelper(Tree *expr, Tree* axiom, std::unordered_map<char, Tree *>& m)
{
    assert(expr && axiom);

    if ('A' <= axiom->tag_[0] && axiom->tag_[0] <= 'Z') {
        Tree*& mt = m[axiom->tag_[0]];
        if (mt == nullptr) {
            mt = expr;
            return true;
        } else {
            return *mt == *expr;
        }
    }

    if (expr->tag_ != axiom->tag_ ||
        expr->children_.size() != axiom->children_.size())
        return false;

    for (size_t i = 0; i < axiom->children_.size(); ++i)
        if (!isAxiomHelper(expr->children_[i], axiom->children_[i], m))
            return false;

    return true;
}

bool Deduction::isFormalAxiom(Tree *expr) const
{
    for (Tree* axiom: formal_axioms_)
        if (*axiom == *expr)
            return true;

    return isInduction(expr);
}

bool Deduction::isInduction(Tree *expr)
{
    // (phi[x:=0]) & @x(phi->phi[x:=x']) -> phi
    if (expr->tag_ == "->") {
        Tree* orig = expr->children_[1]; // phi
        expr = expr->children_[0];       // (phi[x:=0]) & @x(phi->phi[x:=x'])
        if (expr->tag_ == "&") {
            Tree* phi0 = expr->children_[0]; // (phi[x:=0])
            Tree* phix = expr->children_[1]; // @x(phi->phi[x:=x'])
            if (phix->tag_ == "@") {
                Tree* var = phix->children_[0]; // x
                std::unique_ptr<Tree> zero(new Tree("0")); // 0
                std::unique_ptr<Tree> var1(new Tree("'", var)); // x'

                phix = phix->children_[1]; // phi->phi[x:=x']
                if (phix->tag_ == "->") {
                    if (*phix->children_[0] == *orig) {
                        phix = phix->children_[1]; // phi[x:=x']

                        std::unordered_multiset<std::string> bounded;
                        bool res= equalsWithSubstitution(orig, phi0, var->tag_, zero.get(), bounded);
                        bounded.clear();
                        return res && equalsWithSubstitution(orig, phix, var->tag_, var1.get(), bounded);
                    }
                }

            }
        }
    }

    return false;
}

bool Deduction::equalsWithSubstitution(Tree *orig, Tree *expr, const std::string &var,
                                       Tree *new_var, std::unordered_multiset<std::string>& bounded)
{
    if (orig->tag_ == var && !bounded.count(orig->tag_))
        return *expr == *new_var;

    if (orig->tag_ != expr->tag_)
        return false;

    if (orig->tag_ == "@" || orig->tag_ == "?")
        bounded.insert(orig->children_[0]->tag_);

    for (size_t i = 0; i < expr->children_.size(); ++i) {
        if (!equalsWithSubstitution(orig->children_[i], expr->children_[i], var, new_var, bounded))
            return false;
    }

    if (orig->tag_ == "@" || orig->tag_ == "?")
        bounded.erase(bounded.find(orig->children_[0]->tag_));

    return true;
}

bool Deduction::isAA(Tree *expr)
{
   return (expr->tag_ == "->") &&
           (*expr->children_[0] == *expr->children_[1]);
}

bool Deduction::isForallAxiom(Tree *expr)
{
    // @x(phi) -> (phi[x:=theta]), theta is free for substitution
    if (expr->tag_ == "->") {
        Tree* lc = expr->children_[0]; // @x(phi)
        Tree* with_theta = expr->children_[1]; // phi[x:=theta]
        if (lc->tag_ == "@") {
            std::string var = lc->children_[0]->tag_; // x
            Tree* orig = lc->children_[1]; // phi

            static std::unordered_multiset<std::string> bounded;
            bounded.clear();
            return isFreeForSubstitution(orig, with_theta, var, bounded);
        }
    }

    return false;
}

bool Deduction::isExistAxiom(Tree *expr)
{
    // (phi[x:=theta]) -> ?x(phi), theta is free for substitution
    if (expr->tag_ == "->") {
        Tree* with_theta = expr->children_[0]; // phi[x:=theta]
        Tree* rc = expr->children_[1]; // ?x(phi)

        if (rc->tag_ == "?") {
            std::string var = rc->children_[0]->tag_; // x
            Tree* orig = rc->children_[1]; // phi

            static std::unordered_multiset<std::string> bounded;
            bounded.clear();

            return isFreeForSubstitution(orig, with_theta, var, bounded);
        }
    }

    return false;
}

bool Deduction::isFreeForSubstitution(Tree *orig, Tree *expr, const std::string &var, std::unordered_multiset<std::string> &bounded)
{
    Tree* phi = nullptr;
    return isFreeForSubstitutionHelper(orig, expr, var, bounded, phi);
}

bool Deduction::isFreeForSubstitutionHelper(Tree *orig, Tree* expr, const std::string &var,
                                            std::unordered_multiset<std::string>& bounded, Tree*& phi)
{
    if (orig->tag_ == var && !bounded.count(var)) {
        static std::unordered_set<std::string> vars;
        static std::unordered_multiset<std::string> unused;

        if (phi == nullptr) {
            vars.clear();
            unused.clear();
            getFreeVars(expr, vars, unused);
            phi = expr;
        } else if (*phi != *expr) {
            return false;
        }

        for (const std::string& v: vars) {
            if (bounded.count(v))
                return false;
        }

        return true;
    }

    if (orig->tag_ != expr->tag_)
        return false;

    if (orig->tag_ == "@" || orig->tag_ == "?")
        bounded.insert(orig->children_[0]->tag_);

    for (size_t i = 0; i < orig->children_.size(); ++i)
        if (!isFreeForSubstitutionHelper(orig->children_[i], expr->children_[i], var, bounded, phi))
            return false;

    if (orig->tag_ == "@" || orig->tag_ == "?")
        bounded.erase(bounded.find(orig->children_[0]->tag_));

    return true;
}

bool Deduction::isVar(Tree *expr)
{
    char f = expr->tag_[0];
    return 'a' <= f && f <= 'z' && (expr->children_.size() == 0);
}

void Deduction::getFreeVars(Tree *expr, std::unordered_set<std::string> &vars,
                            std::unordered_multiset<std::string> &bounded)
{
    if (isVar(expr) && !bounded.count(expr->tag_)) {
        vars.insert(expr->tag_);
        return;
    }

    if (expr->tag_ == "@" || expr->tag_ == "?")
        bounded.insert(expr->children_[0]->tag_);

    for (Tree* child: expr->children_)
        getFreeVars(child, vars, bounded);

    if (expr->tag_ == "@" || expr->tag_ == "?")
        bounded.erase(bounded.find(expr->children_[0]->tag_));
}

