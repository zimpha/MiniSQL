#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <set>
#include <string>
#include <vector>

struct AttrType {
    std::string name;
    int type, length;
    bool unique;
    std::set<std::string> indices;
    bool operator == (const AttrType &rhs) const;
};

struct Table {
    int entrySize;
    std::string name;
    std::vector<AttrType> attributes;
    int blockCount;
    Table (const std::string &name, const std::vector<AttrType> &attributes);
    Table ();
    int getIndexID(const AttrType &x);
    void write();
};

struct element {
    int i;
    float f;
    std::string s;
    int type; // 0: int, 1: float, 2: string, -1:invalid
    element();
    element(int x);
    element(double x);
    element(float x);
    element(std::string x);
    element(const char *d);
    bool operator < (const element &rhs) const;
    bool operator == (const element &rhs) const;
    bool operator > (const element &rhs) const;
    bool operator >= (const element &rhs) const;
    bool operator <= (const element &rhs) const;
    bool operator != (const element &rhs) const;
    void print() const;
};

struct Response {
    bool succeed;
    std::string info;
    std::vector<std::vector<element> > result;
    Response();
    Response(std::string data);
    Response(std::vector<std::vector<element> > res);
};

#endif