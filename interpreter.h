#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <string>
#include "API.h"

class Interpreter {
public:
    void process(std::string line);
private:
    API api;
    void execfile(std::string filename);
    element parseElement(std::string data);
    void parse(std::string input);
    void printSelectResult(const Table &nt, const Response &res);
};
#endif