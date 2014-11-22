#include "interpreter.h"
#include <iostream>
#include <string>

struct A {
    void run() {
        std::string buf;
        Interpreter ip;
        do {
            std::cout << ">> ";
            std::getline(std::cin, buf);
            bool flag = ip.process(buf);
            if (flag) break;
        } while (1);
    }
};

int main() {
    A *ui = new A;
    ui->run();
    delete ui;
    return 0;
}
