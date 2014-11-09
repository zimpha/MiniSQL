#include <vector>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "API.h"
#include "CatalogManager.h"
//#include "RecordManager.h"
#include "bufferManager.h"
//#include "btree.h"

Response API::createIndex(const std::string &indexName, const std::string &tableName, const std::string &AttrName) {
}

Response API::dropIndex(const std::string &indexName) {
}

Response API::createTable(const std::string &tableName, std::vector<AttrType> &data, int pk) {
}

Response API::dropTable(const std::string &tableName) {
}

Response API::Select(const std::string &tableName, const Filter &filter) {
}

Response API::Delete(const std::string &tableName, const Filter &filter) {
}

Response API::Insert(const std::string &tableName, const vector<element> entry) {
}