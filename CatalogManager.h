#ifndef _CATALOG_MANAGER_H
#define _CATALOG_MANAGER_H

#include <string>
#include <algorithm>
#include "global.h"

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
};

#endif