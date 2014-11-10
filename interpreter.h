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
};
#endif