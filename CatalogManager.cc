#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "global.h"
#include "CatalogManager.h"
#include "bufferManager.h"
#include "RecordManager.h"

bool CatalogManager::hasTable(const std::string &tableName) {
    std::ifstream fin((tableName).c_str());
    if (!fin) return false;
    std::string header;
    fin >> header;
    fin.close();
    return tableName == header;
}

Table CatalogManager::createTable(const std::string &tableName, const std::vector<AttrType> &attrs) {
    assert(!hasTable(tableName));
    Table nt(tableName, attrs);
    nt.write();
    return nt;
}

Table CatalogManager::loadTable(const std::string &tableName) {
    assert(hasTable(tableName));
    Table nt;
    int attrSum;
    std::ifstream fin((tableName).c_str());
    fin >> nt.name >> nt.entrySize >> attrSum >> nt.blockCount;
    for (int i = 0, n; i < attrSum; ++ i) {
        AttrType attr;
        fin >> attr.name >> attr.type >> attr.length >> attr.unique;
        fin >> n;
        for (int j = 0; j < n; ++ j) {
            std::string index;
            fin >> index;
            attr.indices.insert(index);
        }
        nt.attributes.push_back(attr);
    }
    return nt;
}

void CatalogManager::dropTable(const std::string &tableName) {
    assert(hasTable(tableName));
    std::string name = tableName.substr(0, tableName.find('.'));
    Table nt = loadTable(tableName);
    std::vector<AttrType> &attr = nt.attributes;
    for (size_t i = 0; i < attr.size(); ++ i) {
        bool flag = false;
        for (auto &x : attr[i].indices) {
            flag = true;
            dropIndex(x);
        }
        if (flag) {
            BFM bm;
            std::string indexFileName = name + "." + attr[i].name + ".index";
            bm.deleteFile(indexFileName);
        }
    }

    remove((tableName).c_str());
}

bool CatalogManager::hasIndex(const std::string &indexName) {
    return IM.find(indexName);
}

bool CatalogManager::createIndex(const std::string &tableName, const std::string &indexName, int attrIndex) {
    if (hasIndex(indexName)) return false;
    IM.insert(indexName, tableName, attrIndex);
    return true;
}

bool CatalogManager::dropIndex(const std::string &indexName) {
    if (!hasIndex(indexName)) return false;
    IM.erase(indexName);
    return true;
}

std::pair<std::string, int> CatalogManager::getIndex(const std::string &indexName) {
    assert(hasIndex(indexName));
    return IM.ask(indexName);
}
