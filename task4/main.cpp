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

#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace ph = boost::phoenix;
namespace args = ph::arg_names;
namespace lam = boost::lambda;

typedef std::string::iterator Iterator;
struct arithm_gram
      : qi::grammar<Iterator, void(), ascii::space_type>
{
    arithm_gram()
        : arithm_gram::base_type(expr)
    {
        using qi::_val;
        using qi::_1;
        using qi::_2;

        using ph::construct;
        using ph::val;

        using namespace qi;

        expr = 
            disj
            >> *("->" >> disj);

        disj = 
            conj
            >> *('|' >> conj);

        conj = 
            unar
            >> *('&' >> unar);

        unar =
            pred
            | '!' >> unar
            | '(' >> expr > ')'
            | '@' >> var >> unar
            | '?' >> var >> unar;

        var = 
            char_("a-z") 
            >> *char_("0-9");

        pred = 
              char_("A-Z") >> 
              *char_("0-9") >> 
              -('(' >> term >> *(',' >> term) > ')')
            | term >> '=' >> term;

        term = 
            add 
            >> *('+' >> add);

        add = 
            mult
            >> *('*' >> mult);

        mult = 
            (char_("a-z") 
             >> *char_("0-9")
             >> '(' >> term >> *(',' >> term) > ')'
             | var
             | '(' >> term > ')'
             | char_('0')
            ) >> *char_('\'');

        expr.name("expr");
        disj.name("disj");
        conj.name("conj");
        unar.name("unar");
        var.name("var");
        pred.name("pred");
        term.name("term");
        add.name("add");
        mult.name("mult");

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
    }

    template<typename T>
    using rule = qi::rule<Iterator, T(), ascii::space_type>;

    typedef rule<void> v_rule;

    v_rule expr;
    v_rule disj;
    v_rule conj;
    v_rule unar;
    v_rule var;
    v_rule pred;
    v_rule term;
    v_rule add;
    v_rule mult;
};

int parse(std::ifstream& in) {
    std::string storage; 
    getline(in, storage);

    std::cerr << "expr: " << storage << std::endl;
    arithm_gram gram;
    auto it = storage.begin();
    bool r = phrase_parse(it, storage.end(), gram, ascii::space);
    std::cerr << "r = " << r << std::endl;
    std::cerr << "unparse: " << std::string(it, storage.end()) << std::endl;

    return r && it == storage.end();    
}


int main() {
    freopen("log", "w", stderr);
    std::ifstream in("in");
    int res = parse(in);
    std::cerr << "result: " << res << std::endl;

    return 0;
}
