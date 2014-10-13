#ifndef checkOnFalse_H
#define checkOnFalse_H

#include <iostream>
#include <vector>
#include "parser.h"

bool getBinOpValue(std::string, bool, bool);
bool getValue(parser::linkOnTree);
bool isValidity(parser::linkOnTree);
void getVariables(parser::linkOnTree);
bool checkOnFalse(parser::linkOnTree);

#endif
