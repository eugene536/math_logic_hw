#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include <string>
#include "tree.h"

Tree* parse(const std::string& expression, std::ostream& log = std::cerr);
std::pair<std::vector<Tree*>, Tree*> parseHeader(std::string header, std::ostream& log);

#endif // PARSER_H
