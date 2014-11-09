#include "interpreter.h"
#include <iostream>
#include <string>

int main() {
    std::string buf;
    Interpreter ip;
    while (getline(cin, buf)) {
        std::cout << ">> ";
        ip.process(buf);
    }
}