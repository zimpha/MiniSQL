#include "interpreter.h"
#include <iostream>
#include <string>

int main() {
    std::string buf;
    Interpreter ip;
    do {
        std::cout << ">> ";
        std::getline(std::cin, buf);
        ip.process(buf);
    } while (1);
}
