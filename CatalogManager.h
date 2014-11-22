#ifndef _CATALOG_MANAGER_H
#define _CATALOG_MANAGER_H
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "global.h"

class IndexTableManager {
public:
    IndexTableManager()
    {
        std::ifstream fin("index.catalog");
        mp.clear();
        if (!fin) return;
        std::string indexName, tableName;
        int attrIndex;
        while (fin >> indexName >> tableName >> attrIndex) {
            mp[indexName] = make_pair(tableName, attrIndex);
        }
        fin.close();
    }
    bool find(const std::string indexName)
    {
        return mp.count(indexName);
    }
    void insert(const std::string &indexName, const std::string &tableName, int attrIndex)
    {
        mp[indexName] = make_pair(tableName, attrIndex);
    }
    std::pair<std::string, int> ask(const std::string indexName)
    {
        return mp[indexName];
    }
    void erase(const std::string indexName)
    {
        mp.erase(mp.find(indexName));
    }
    ~IndexTableManager()
    {
        std::ofstream fout("index.catalog");
        for (auto &x : mp) {
            fout << x.first << " " << x.second.first << " " << x.second.second << std::endl;
        }
        fout.close();
        mp.clear();
    }
private:
    std::map<std::string, std::pair<std::string, int> > mp;
};

class CatalogManager {
public:
    bool hasTable(const std::string &tableName);
    Table createTable(const std::string &tableName, const std::vector<AttrType> &attrs);
    Table loadTable(const std::string &tableName);
    void dropTable(const std::string &tableName);
    bool hasIndex(const std::string &indexName);
    bool createIndex(const std::string &tableName, const std::string &indexName, int attrIndex);
    bool dropIndex(const std::string &indexName);
    std::pair<std::string, int> getIndex(const std::string &indexName);
    IndexTableManager IM;
};

#endif
