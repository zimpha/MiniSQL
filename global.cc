#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include "global.h"

Table::Table() {
}

Table::Table (const std::string &name, const std::vector<AttrType> &attributes) {
    this->entrySize = 0;
    this->name = name;
    this->blockCount = 1;
    for (size_t i = 0; i < attributes.size(); ++ i) {
        switch (attributes[i].type) {
            case 0: entrySize += sizeof(int); break;
            case 1: entrySize += sizeof(float); break;
            case 2: entrySize += attributes[i].length; break;
            default: assert(false);
        }
    }
    this->attributes = attributes;
}

void Table::write() {
    std::ofstream fout((name).c_str());
    fout << name << std::endl;
    fout << entrySize << std::endl;
    fout << attributes.size() << std::endl;
    fout << blockCount << std::endl;
    for (size_t i = 0; i < attributes.size(); ++ i) {
        fout << attributes[i].name << std::endl;
        fout << attributes[i].type << std::endl;
        fout << attributes[i].length << std::endl;
        fout << attributes[i].unique << std::endl;
        fout << attributes[i].indices.size() << std::endl;
        std::set<std::string> &indices = attributes[i].indices;
        for (auto &x : indices) {
            fout << x << std::endl;
        }
        fout << std::endl;
    }
    fout.close();
}

int Table::getIndexID(const AttrType &x) {
    for (size_t i = 0; i < attributes.size(); ++ i) {
        if (attributes[i].name == x.name) return i;
    }
    return -1;
}

bool AttrType::operator == (const AttrType &rhs) const {
    return type == rhs.type && length == rhs.length && unique == rhs.unique &&
           indices == rhs.indices && name == rhs.name;
}

element::element(): type(-1) { }
element::element(int x): i(x), type(0) {}
element::element(float x): f(x), type(1) {}
element::element(double x): f(x), type(1) {}
element::element(std::string x): s(x), type(2) {}
element::element(const char *x): s(x), type(2) {}

void element::print() const {
    switch (type) {
        case 0: std::cout << i; return;
        case 1: std::cout << f; return;
        case 2: std::cout << s; return;
        default: assert(false);
    }
}

bool element::operator < (const element &rhs) const {
    assert(type == rhs.type);
    switch (type) {
        case 0: return i < rhs.i;
        case 1: return f < rhs.f;
        case 2: return s < rhs.s;
        default: assert(false);
    }
}

bool element::operator == (const element &rhs) const {
    assert(type == rhs.type);
    switch (type) {
        case 0: return i == rhs.i;
        case 1: return fabs(f - rhs.f) < 1e-8;
        case 2: return s == rhs.s;
        default: assert(false);
    }
}

bool element::operator > (const element &rhs) const {
    return !(*this < rhs || *this == rhs);
}

bool element::operator >= (const element &rhs) const {
    return *this > rhs || *this == rhs;
}

bool element::operator <= (const element &rhs) const {
    return *this < rhs || *this == rhs;
}

bool element::operator != (const element &rhs) const {
    return !(*this == rhs);
}

Response::Response(): succeed(true) {}
Response::Response(std::string data): succeed(false), info(data) {}
Response::Response(std::vector<std::vector<element> > res): succeed(true), result(res) {}
