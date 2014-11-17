#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include "global.h"
#include "bufferManager.h"

struct IndexIter;

struct IndexManager {
    Reponse create(const std::string &indexName, const Table &table, const AttrType &attr);

    Reponse drop(const std::string &indexName);

    bool check(const Table &table, const AttrType &attr);

    Response insert(const std::string &indexName, const element &e, long offset);

    Response erase(const std::string &indexName, const element &e);

    //如果没有，返回isEnd()为true的迭代器，类似STL左闭右开
    IndexIter find(const std::string &indexName, const element &e);

    IndexIter lower_bound(const std::string &indexName, const element &e);

    IndexIter upper_bound(const std::string &indexName, const element &e);

private:
    BFM bfm;
};

struct IndexIter {
    bool isBegin();

    bool isEnd();

    void set(long offset);

    long get();

    element getKey();

    IndexIter& operator ++();

    IndexIter operator ++(int);

    IndexIter& operator --();

    IndexIter operator --(int);

private:
    element key;
    long value;
    long treeNodeOffset;
};

#endif