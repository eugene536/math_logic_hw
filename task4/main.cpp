#include <boost/spirit/include/qi.hpp>

namespace client {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template<typename Iterator>
    bool ParseNumbers(Iterator begin, Iterator end) {
        using qi::double_;
        using qi::phrase_parse;
        using ascii::space;

        bool r = phrase_parse(begin, end, double_ >> *(',' >> double_), space);
        
        return begin == end && r;
    }
}

int main() {
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tA comma separated list parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout << "Give me a comma separated list of numbers.\n";
    std::cout << "Type [q or Q] to quit\n\n";

    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        if (client::ParseNumbers(str.begin(), str.end()))
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << str << " Parses OK: " << std::endl;
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
