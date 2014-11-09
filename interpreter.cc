#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <strstream>
#include "global.h"
#include "CatalogManager.h"
#include "interpreter.h"

void Interpreter::process(std::string line) {
    string cmd;
    for (size_t i = 0; i < line.length(); ++ i) {
        if (line[i] == ';') {
            string now(cmd);
            cmd = "";
            parseCmd(now);
        }
        else cmd += line[i];
    }
}