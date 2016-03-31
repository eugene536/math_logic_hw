#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/config/warning_disable.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>

#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/statement/if.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "tree.h"

namespace {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace ph = boost::phoenix;
    namespace args = ph::arg_names;
    namespace lam = boost::lambda;

    typedef std::string::const_iterator Iterator;
    struct FormalGrammar
    : qi::grammar<Iterator, Tree*(), ascii::space_type> {
        FormalGrammar(std::ostream& log)
            : FormalGrammar::base_type(expr) {
            using qi::_val;
            using qi::_1;
            using qi::_2;

            using ph::construct;
            using ph::val;
            using ph::if_;
            using ph::new_;

            using namespace qi;

            expr =
                (disj >> -("->" >> expr))            [
                                                       if_ (_2) [
                                                          _val = new_<Tree>("->", _1, _2)
                                                       ] .else_ [
                                                          _val = _1
                                                       ]
                                                     ];

            disj =
                (
                    conj                             [_a = _1]
                    >> *('|' >> conj                 [_a = new_<Tree>("|", _a, _1)])
                )                                    [_val = _a];

            conj =
                (
                    unar                             [_a = _1]
                    >> *('&' >> unar                 [_a = new_<Tree>("&", _a, _1)])
                )                                    [_val = _a];


            unar =
                '!' >> unar                          [_val = new_<Tree>("!", _1)]
                | ('(' >> expr > ')')                [_val = _1]
                | ('@' >> var >> unar)               [_val = new_<Tree>("@", new_<Tree>(_1), _2)]
                | ('?' >> var >> unar)               [_val = new_<Tree>("?", new_<Tree>(_1), _2)]
                | pred                               [_val = _1];

            var %=
                char_("a-z")
                >> *char_("0-9");

            pred =
                (
                    char_("A-Z") >>
                    *char_("0-9") >>
                    -('(' >> (term % ',') > ')')
                )                                    [_val = new_<Tree>(_1, _2, _3)]
                | (term >> '=' >> term)              [_val = new_<Tree>("=", _1, _2)];

            term =
                (
                    add                              [_a = _1]
                    >> *('+' >> add                  [_a = new_<Tree>("+", _a, _1)])
                )                                    [_val = _a];

            add =
                (
                    mult                             [_a = _1]
                    >> *('*' >> mult                 [_a = new_<Tree>("*", _a, _1)])
                )                                    [_val = _a];

            func =
                (
                    var >> '(' >> (term % ',') > ')'
                )                                    [_val = new_<Tree>(_1, _2)];

            zeroTree = char_('0')                    [_val = new_<Tree>("0")];

            varTree = var                            [_val = new_<Tree>(_1)];

            mult =
                (
                    ((func                           [_a = _1])
                       | ('(' >> term > ')')         [_a = _1]
                       | varTree                     [_a = _1]
                       | zeroTree                    [_a = _1]
                    ) >> * (char_('\'')              [_a = new_<Tree>("'", _a)])
                )                                    [_val = _a];

            expr.name("expr");
            disj.name("disj");
            conj.name("conj");
            unar.name("unar");
            var.name("var");
            pred.name("pred");
            term.name("term");
            add.name("add");
            mult.name("mult");
            zeroTree.name("zeroTree");
            varTree.name("varTree");
            func.name("func");

            on_error<fail>
            (
                expr
                , log
                << val("Error! Expecting ")
                << _4                               // what failed?
                << val(" here: \"")
                << construct<std::string>(_3, _2)   // iterators to error-pos, end
                << val("\"")
                << std::endl
            );

//            debug(expr);
//            debug(disj);
//            debug(conj);
//            debug(unar);
//            debug(var);
//            debug(pred);
//            debug(term);
//            debug(add);
//            debug(mult);
//            debug(zeroTree);
//            debug(varTree);
//            debug(func);
        }

        template<typename T>
        using rule = qi::rule<Iterator, T(), ascii::space_type>;

        template<typename T, typename U>
        using rule_locals = qi::rule<Iterator, T(), qi::locals<U>, ascii::space_type>;

        rule<Tree *> expr;
        rule_locals<Tree *, Tree *> disj;
        rule_locals<Tree *, Tree *> conj;
        rule<Tree *> unar;
        rule<Tree *> pred;
        rule_locals<Tree *, Tree *> term;
        rule_locals<Tree *, Tree *> add;
        rule_locals<Tree *, Tree *> mult;
        rule_locals<Tree *, std::string> func;
        rule<std::string> var;
        rule<Tree *> varTree;
        rule<Tree *> zeroTree;
    };
}

Tree* parse(const std::string& expression, std::ostream& log) {
    FormalGrammar gram(log);
    auto it = expression.cbegin();
    Tree* res = nullptr;
    bool r = phrase_parse(it, expression.cend(), gram, ascii::space, res);

    if (it != expression.cend())
        log << "unparse: " << std::string(it, expression.cend()) << std::endl;

    if (r && it == expression.cend()) {
        return res;
    }

    return nullptr;
}

std::pair<std::vector<Tree*>, Tree*> parseHeader(std::string header, std::ostream& log) {
    boost::algorithm::trim(header);

    FormalGrammar gram(log);
    std::vector<Tree*> context;

    Tree* res;
    auto beg = header.cbegin();
    auto it = header.cbegin();

    if (*it == '|') { // if context is empty
        it++;
        if (it != header.cend() && *it == '-')
            it++;
        else {
            log << "unparse: " << std::string(header.cbegin(), header.cend()) << std::endl;
            return {context, nullptr};
        }
    }

    while (it != header.cend()) {
        res = nullptr;
        beg = it;
        bool r = phrase_parse(it, header.cend(), gram, ascii::space, res);

        if (!r) {
            log << "unparse: " << std::string(beg, header.cend()) << std::endl;
            return {context, nullptr};
        }

        if (it == header.cend())
            break;

        if (*it == ',' || *it == '|') {
            context.push_back(res);

            if (*it++ == '|') {
                if (it != header.cend() && *it == '-') {
                    ++it;
                } else {
                    log << "unparse: " << std::string(beg, header.cend()) << std::endl;
                    return {context, nullptr};
                }
            }
        } else {
            log << "unparse: " << std::string(beg, header.cend()) << std::endl;
            return {context, nullptr};
        }
    }

    if (it == header.cend()) {
        return {context, res};
    } else
        log << "unparse: " << std::string(beg, header.cend()) << std::endl;

    return {context, nullptr};
}
