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
#include <boost/phoenix/bind/bind_function_object.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/statement/if.hpp>

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

    typedef std::string::iterator Iterator;
    struct FormalGrammar
    : qi::grammar<Iterator, Tree*(), ascii::space_type> {
        FormalGrammar()
            : FormalGrammar::base_type(expr) {
            using qi::_val;
            using qi::_1;
            using qi::_2;

            using ph::construct;
            using ph::val;
            using ph::if_;

            using namespace qi;

            expr =
                (disj >> -("->" >> expr))              [
                                                         if_ (_2) [
                                                            _val = ph::new_<Tree>("->", _1, _2)
                                                         ] .else_ [
                                                            _val = _1
                                                         ]
                                                       ];

            disj =
                (
                    conj                               [_a = _1]
                    >> *('|' >> conj                   [_a = ph::new_<Tree>("|", _a, _1)])
                )                                      [_val = _a];

            conj =
                (
                    unar                               [_a = _1]
                    >> *('&' >> unar                   [_a = ph::new_<Tree>("&", _a, _1)])
                )                                      [_val = _a];


            unar =
                '!' >> unar                            [_val = ph::new_<Tree>("!", _1)]
                | ('(' >> expr > ')')                  [_val = _1]
                | ('@' >> var >> unar)                 [_val = ph::new_<Tree>("@", _1, _2)]
                | ('?' >> var >> unar)                 [_val = ph::new_<Tree>("?", _1, _2)]
                | pred                                 [_val = _1];

            var %=
                char_("a-z")
                >> *char_("0-9");

            pred =
                (
                    char_("A-Z") >>
                    *char_("0-9") >>
                    -('(' >> (term % ',') > ')')
                )                                      [_val = ph::new_<Tree>(_1, _2, _3)]
                | (term >> '=' >> term)                [_val = ph::new_<Tree>("=", _1, _2)];

            term =
                (
                    add                                [_a = _1]
                    >> *('+' >> add                    [_a = ph::new_<Tree>("+", _a, _1)])
                )                                      [_val = _a];

            add =
                (
                    mult                               [_a = _1]
                    >> *('*' >> mult                   [_a = ph::new_<Tree>("*", _a, _1)])
                )                                      [_val = _a];

            func =
                (
                    var >> '(' >> (term % ',') > ')'
                )                                      [_val = ph::new_<Tree>(_1, _2)];

            zeroTree = char_('0')                      [_val = ph::new_<Tree>("0")];

            varTree = var                              [_val = ph::new_<Tree>(_1)];

            mult =
                (
                    ((func                             [_val = _1])
                       | ('(' >> term > ')')           [_val = _1]
                       | varTree                       [_val = _1]
                       | zeroTree                      [_val = _1]
                    ) >> *char_('\'')
                )                                      [
                                                         if_ (ph::size(_2)) [
                                                             _val = ph::new_<Tree>(_2, _1)
                                                         ] .else_ [
                                                             _val = _1
                                                         ]
                                                       ];

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
                , std::cerr
                << val("Error! Expecting ")
                << _4                               // what failed?
                << val(" here: \"")
                << construct<std::string>(_3, _2)   // iterators to error-pos, end
                << val("\"")
                << std::endl
            );

            debug(expr);
            debug(disj);
            debug(conj);
            debug(unar);
            debug(var);
            debug(pred);
            debug(term);
            debug(add);
            debug(mult);
            debug(zeroTree);
            debug(varTree);
            debug(func);
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
        rule<Tree *> mult;
        rule_locals<Tree *, std::string> func;
        rule<std::string> var;
        rule<Tree *> varTree;
        rule<Tree *> zeroTree;
    };
}

Tree* parse(std::string expression) {
    FormalGrammar gram;
    auto it = expression.begin();
    Tree* res = nullptr;
    bool r = phrase_parse(it, expression.end(), gram, ascii::space, res);

    if (it != expression.end())
        std::cerr << "unparse: " << std::string(it, expression.end()) << std::endl;

    if (r && it == expression.end())
        return res;

    return nullptr;
}
