#include <cstdio>
#include <cassert>
#include "filter.h"

Rule::Rule() {}
Rule::Rule(int idx, int t, element x):
    index(idx), type(t), val(x) {}

Filter::Filter() {}

void Filter::addRule(const Rule &rule) {
    rules.push_back(rule);
}

bool Filter::test(const std::vector<element> &data) const {
    for (size_t i = 0; i < rules.size(); ++ i) {
        const Rule &rule = rules[i];
        assert((size_t)rule.index < data.size());
        switch (rule.type) {
            case 0:
                if (!(data[rule.index] <  rule.val)) return false;
                break;
            case 1:
                if (!(data[rule.index] <= rule.val)) return false;
                break;
            case 2:
                if (!(data[rule.index] == rule.val)) return false;
                break;
            case 3:
                if (!(data[rule.index] >= rule.val)) return false;
                break;
            case 4:
                if (!(data[rule.index] >  rule.val)) return false;
                break;
            case 5:
                if (!(data[rule.index] != rule.val)) return false;
                break;
            default: 
                assert(false);
        }
    }
    return true;
}

//#define LOCAL_TEST
#ifdef LOCAL_TEST
int main() {
    return 0;
}
#endif