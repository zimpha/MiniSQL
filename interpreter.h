#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <string>
#inlcude "API.h"

class Interpreter {
public:
    void process(std::string line);
private:
    API api;
};
#endif