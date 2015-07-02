#ifndef checkOnFalse_H
#define checkOnFalse_H

#include <iostream>
#include <string.h>
#include <stdexcept>
#include <cmath>
#include "deduction.h" // pars, vect

bool getBinOpValue(std::string, bool, bool);
bool getValue(parser::linkOnTree);
bool isValidity(parser::linkOnTree);
void getVariables(parser::linkOnTree);
void checkOnFalse(parser::linkOnTree);

#endif
