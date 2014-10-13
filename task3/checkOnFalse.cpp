#include "checkOnFalse.h"

std::unordered_map<std::string, bool> badAnswer;
std::vector<std::string> variables;

bool getBinOpValue(std::string s, bool x, bool y) {
    if (s == "&")
        return x & y;
    else if (s == "|")
        return x | y;
    else if (s == "->")
        return !x | y;
    else //if (s == "!")
        return !x;
}

bool getValue(parser::linkOnTree cur) {
    // for !(NOT)
    if (!cur) 
        return false;

    if (goodCharForVar(cur->str[0])) {
        return badAnswer[cur->str];
    }

    return getBinOpValue(cur->str, getValue(cur->left), getValue(cur->right));
}

bool isValidity(parser::linkOnTree cur) {
    int sz = variables.size();
    bool f = true;
    for (int i = 0; i < (1 << sz); i++) {
        //choose appropriate bits
        for (int j = 0; j < sz; j++)
            badAnswer[variables[j]] = i & (1 << j);
        f = getValue(cur);
        if (!f)
            return false;
    }
    return true;
}

//PRE: cur != NULL
void getVariables(parser::linkOnTree cur) {
    if (!cur->left && !cur->right) {
        if (badAnswer.count(cur->str) == 0) {
            badAnswer[cur->str] = 0;
            variables.push_back(cur->str);
        }
        return;
    }
    getVariables(cur->left);
    if (cur->right)
        getVariables(cur->right);
}

bool checkOnFalse(parser::linkOnTree cur) {
    getVariables(cur);
    bool f = isValidity(cur);
    if (!f) {
        for (auto it : variables)
            std::cout << it << " = " << badAnswer[it] << std::endl;
    }
    return f;
}
