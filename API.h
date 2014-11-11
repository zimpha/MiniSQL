#ifndef _API_H
#define _API_H

#include <string>
#include <vector>
#include "global.h"
#include "filter.h"
#include "CatalogManager.h"
#include "bufferManager.h"
#include "RecordManager.h"

class API {
public:
    Response createIndex(const std::string &indexName, const std::string &tableName, const std::string &attrName);
    Response dropIndex(const std::string &indexName);
    Response createTable(const std::string &tableName, std::vector<AttrType> &data, int pk);
    Response dropTable(const std::string &tableName);
    Response Select(const std::string &tableName, const Filter &filter);
    Response Delete(const std::string &tableName, const Filter &filter);
    Response Insert(const std::string &tableName, const std::vector<element> entry);
    CatalogManager cm;
    RecordManager rm;
    BFM bm;
};

#endif