#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <string>
#include "API.h"

class Interpreter {
public:
    bool process(std::string line);
private:
    API api;
    bool execfile(std::string filename);
    element parseElement(std::string data);
    bool parse(std::string input);
    void printSelectResult(const Table &nt, const Response &res);
};
#endif