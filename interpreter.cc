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
    static string cmd;
    for (size_t i = 0; i < line.length(); ++ i) {
        if (line[i] == ';') {
            string now(cmd);
            cmd = "";
            parse(now);
        }
        else cmd += line[i];
    }
}

void Interpreter::execfile(std::string filename) {
    std::ifstream fin(filename.c_str());
    if (!fin) cout << "no such file" << endl;
    else {
        std::string buf;
        while (getline(fin, buf)) {
            int len = buf.size();
            while (len && buf[len - 1] == '\n' || buf[len - 1] == '\r') {
                len --;
            }
            process(buf.substr(0, len));
        }
    }
}