#include <vector>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "API.h"
#include "CatalogManager.h"
//#include "RecordManager.h"
//#include "bufferManager.h"
//#include "btree.h"

// File name rule:
// Table: $TableName.table
// Index: $TableName.$ItemName.index
// DB File: $TableName.db

Response API::createIndex(const std::string &indexName, const std::string &tableName, const std::string &attrName) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    Table nt = cm.loadTable(tableName + ".table");
    int attrIndex = -1;
    for (size_t i = 0; i < nt.attributes.size(); ++ i) {
        if (nt.attributes[i].name == attrName) {
            attrIndex = i;
            break;
        }
    }
    if (attrIndex == -1) {
        return Response("Attribute does not exist");
    }
    if (!nt.attributes[attrIndex].unique) {
        return Response("This attribute must be unique");
    }
    if (!cm.createIndex(tableName, indexName, attrIndex)) {
        return Response("This index already exists");
    }
    nt.attributes[attrIndex].indices.insert(indexName);
    nt.write();
    if (nt.attributes[attrIndex].indices.size() == 1) {
        // Record Manager add index
        // 提供index文件名和当前表，以及attrIndex给Record Manager
        // rmAddIndex(tableName+".db",tableName+"."+nt.attributes[attrIndex].name+".index",nt,attrIndex);
    }
    return Response();
}

Response API::dropIndex(const std::string &indexName) {
    if (!cm.hasIndex(indexName)) {
        return Response("Index does not exist");
    }
    std::pair<std::string, int> psi = cm.getIndex(indexName);
    std::string tableName = psi.first;
    int attrIndex = psi.second;
    Table nt = cm.loadTable(tableName + ".table");
    assert(nt.attributes[attrIndex].indices.count(indexName));
    nt.attributes[attrIndex].indices.erase(indexName);
    nt.write();
    cm.dropIndex(indexName);
    if (nt.attributes[attrIndex].indices.size() == 0) {
        // Buffer Manager remove index
        // bmClear(tableName+"."+nt.attributes[attrIndex].name+".index");
    }
    return Response();
}

Response API::createTable(const std::string &tableName, std::vector<AttrType> &data, int pk) {
    if (cm.hasTable(tableName + ".table")) {
        return Response("This table already exists");
    }
    
    Table nt = cm.createTable(tableName + ".table", data);
    nt.attributes[pk].unique = true;
    nt.write();
    
    return createIndex(tableName + ".PrimaryKeyDefault", tableName, nt.attributes[pk].name);
}

Response API::dropTable(const std::string &tableName) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    cm.dropTable(tableName+".table");
    return Response();
}

Response API::Select(const std::string &tableName, const Filter &filter) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    
    return Response();
}

Response API::Delete(const std::string &tableName, const Filter &filter) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    
    return Response();
}

Response API::Insert(const std::string &tableName, const std::vector<element> entry) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    const Table nt = cm.loadTable(tableName + ".table");
    std::string dbName = tableName + ".db";
    if (nt.attributes.size() != entry.size()) {
        return Response("Type mismatch");
    }
    
    for (size_t i = 0; i < nt.attributes.size(); ++ i) {
        if (nt.attributes[i].unique) {
            Filter filter;
            filter.addRule(Rule(i, 2, entry[i]));
            Response tmp = Select(tableName, filter);
            if (!tmp.result.empty()) {
                return Response("unique integrity violation");
            }
        }
    }
    // Record Manager insert record
    // rmInsertRecord(tableName+".db", entry, nt);
    return Response();
}