#ifndef _FILTER_H
#define _FILTER_H

#include "global.h"
#include <vector>

struct Rule {
    int index, type; // 0: <, 1: <=, 2: =, 3: >=, 4: >, 5: <>
    element val;
    Rule();
    Rule(int idx, int t, element x);
};

struct Filter {
    std::vector<Rule> rules;
    Filter();
    void addRule(const Rule &rule);
    bool test(const std::vector<element> &data) const;
};

#endif