#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include "global.h"
#include "bufferManager.h"
#include <string>
#include <set>
#include <map>

struct IndexIter;
struct Node;

struct IndexManager {
    IndexManager(BFM &);

    ~IndexManager();

    Response create(const std::string &indexName, const std::string &dbName, Table &table, const AttrType &attr);

    Response drop(const std::string &indexName);

    Response insert(const std::string &indexName, const element &e, long offset);

    Response erase(const std::string &indexName, const element &e);

    //返回对应offset, -1表示没找到
    long find(const std::string &indexName, const element &e);

    std::set <long> greater(const std::string &indexName, const element &e);

    std::set <long> less(const std::string &indexName, const element &e);

    //返回[lhs, rhs]这个区间的内容
    std::set <long> inRange(const std::string &indexName, const element &lhs, const element &rhs);

    void load();
    void save();

private:
    BFM &bfm;
    std::map <element, long> mp;
    std::string currentFile;
};

#endif