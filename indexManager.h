#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include "global.h"

struct IndexIter;

struct IndexManager {

    Reponse create(const std::string &indexName, const Table &table, const AttrType &attr);

    Reponse drop(const std::string &indexName);

    bool check(const Table &table, const AttrType &attr);

    Response insert(const std::string &indexName, const element &e, long offset);

    Response erase(const std::string &indexName, const element &e);

    //如果没有throw "not exist"
    IndexIter find(const std::string &indexName, const element &e);

    //如果没有throw "not exist"
    IndexIter lower_bound(const std::string &indexName, const element &e);

    //如果没有throw "not exist"
    IndexIter upper_bound(const std::string &indexName, const element &e);
};

struct IndexIter {
    bool hasFront();
    bool hasNext();
    IndexIter& operator ++();
    IndexIter operator ++(int);
    IndexIter& operator --();
    IndexIter operator --(int);
};

#endif