#include <vector>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "API.h"

// File name rule:
// Table: $tableName.table
// Index: $tableName.$attrName.index
// DB File: $tableName.db

API::API() :rm(bm)
{
}

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
        // 这个是给Index Manager的吧？
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
        bm.deleteFile(tableName + "." + nt.attributes[attrIndex].name + ".index");
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

    std::string dbName = tableName + ".db";
    // Record Manager creat database
    // 传入数据库文件名(dbName)
    rm.RecordManagerTableCreate(dbName);

    return createIndex(tableName + ".PrimaryKeyDefault", tableName, nt.attributes[pk].name);
}

Response API::dropTable(const std::string &tableName) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    cm.dropTable(tableName+".table");

    std::string dbName = tableName + ".db";
    // Record Manager clear database
    // 传入数据库文件名(dbName)
    rm.RecordManagerTableDetete(dbName);

    return Response();
}

Response API::Select(const std::string &tableName, const Filter &filter) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    
    Table nt = cm.loadTable(tableName + ".table");
    std::string dbName = tableName + ".db";
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        element val = filter.rules[i].val;
        int attrIndex = filter.rules[i].index;
        if (nt.attributes[attrIndex].type != val.type) {
            return Response("Type mismatch");
        }
    }
    
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        if (filter.rules[i].type == 2) { // =
            element val = filter.rules[i].val;
            int attrIndex = filter.rules[i].index;
            if (!nt.attributes[attrIndex].indices.empty()) { // has index on it
                std::string indexFileName = tableName + "." + nt.attributes[attrIndex].name + ".index";
                long offset = getOffset(indexFileName, val);
                // offset == -1 表示没有这个val
                // Record Manager return select result by using index
                // 传入数据库文件名（dbName），偏移量(offset)，过滤器(filter)，当前表(nt)，返回select结果(类型vector<vector<element>>)
                return Response(rm.RecordManagerRecordSelect(dbName, offset, filter, nt));
            }
        }
    }

    // SelectWithoutIndex is just an exceptional case of SelectWithIndex
    /*
    if (filter.rules.empty()) {
        // Record Manager return select result without index
        // 传入数据库文件名（dbName），过滤器(filter)，当前表(nt)，返回select结果(类型vector<vector<element>>)
        return Response(rm.rmSelectWithoutIndex(dbName, filter, nt));
    }
    */

    std::set<long> offset;
    // Record Manager get offset according to dbName
    // 传入数据库文件名(daName)，返回一个offset的集合(类型set)
    offset = rm.RecordManagerGetAllOffsets(dbName);
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        Rule rule = filter.rules[i];
        int attrIndex = rule.index;
        if (nt.attributes[attrIndex].indices.empty()) {
            continue;
        }
        element val = rule.val;
        std::string indexFileName = tableName + "." + nt.attributes[attrIndex].name + ".index";
        std::set<long> newOffset;
        long tmpOffset;
        // B plus tree get offset according to filter rules
        // 传入文件名(indexFileName)，属性的值(val)
        switch (rule.type) {
            case 0: // <
                newOffset = getLessOffset(indexFileName, val);
                break;
            case 1: // <=
                newOffset = getLessOffset(indexFileName, val);
                tmpOffset = getOffset(indexFileName, val);
                if (tmpOffset != -1) {
                    newOffset.insert(tmpOffset);
                }
                break;
            case 2: // =
                assert(false);
                break;
            case 3: // >=
                newOffset = getMoreOffset(indexFileName, val);
                tmpOffset = getOffset(indexFileName, val);
                if (tmpOffset != -1) {
                    newOffset.insert(tmpOffset);
                }
                break;
            case 4: // >
                newOffset = getMoreOffset(indexFileName, val);
                break;
            case 5: // <>
                break;
            default: 
                assert(false);
                break;
        }
        std::vector<long> tmp(offset.size());
        std::vector<long>::iterator end = std::set_intersection(offset.begin(), offset.end(), newOffset.begin(), newOffset.end(), tmp.begin());
        offset.clear();
        for (std::vector<long>::iterator it = tmp.begin(); it != end; ++ it) {
            offset.insert(*it);
        }
    }
    
    std::vector<std::vector<element> > res;
    for (auto x : offset) {
        std::vector<std::vector<element> > tmp;
        // Record Manager return select result by using index
        // 传入数据库文件名（dbName），偏移量(x)，过滤器(filter)，当前表(nt)，返回select结果(类型Response)
        tmp = rm.RecordManagerRecordSelect(dbName, x, filter, nt);
        for (size_t i = 0; i < tmp.size(); ++ i) {
            res.push_back(tmp[i]);
        }
    }
    return Response(res);
}

Response API::Delete(const std::string &tableName, const Filter &filter) {
    if (!cm.hasTable(tableName + ".table")) {
        return Response("Table does not exist");
    }
    
    const Table nt = cm.loadTable(tableName + ".table");
    std::string dbName = tableName + ".db";
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        element val = filter.rules[i].val;
        int attrIndex = filter.rules[i].index;
        if (nt.attributes[attrIndex].type != val.type) {
            return Response("Type mismatch");
        }
    }
    
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        if (filter.rules[i].type == 2) { // =
            element val = filter.rules[i].val;
            int attrIndex = filter.rules[i].index;
            if (!nt.attributes[attrIndex].indices.empty()) { // has index on it
                std::string indexFileName = tableName + "." + nt.attributes[attrIndex].name + ".index";
                long offset = getOffset(indexFileName, val);
                // offset == -1 表示没有这个val
                // Record Manager delete records by using index
                // 传入数据库文件名（dbName），偏移量(offset)，过滤器(filter)，当前表(nt)
                rm.RecordManagerRecordDelete(dbName, offset, filter, nt);
                return Response();
            }
        }
    }
    /*
    if (filter.rules.empty()) {
        // Record Manager delete records without index
        // 传入数据库文件名（dbName），过滤器(filter)，当前表(nt)
        rm.rmDeleteWithoutIndex(dbName, filter, nt);
        return Response();
    }
    */
    std::set<long> offset;
    // Record Manager get offset according to dbName
    // 传入数据库文件名(daName)，返回一个offset的集合(类型set)
    offset = rm.RecordManagerGetAllOffsets(dbName);
    for (size_t i = 0; i < filter.rules.size(); ++ i) {
        Rule rule = filter.rules[i];
        int attrIndex = rule.index;
        if (nt.attributes[attrIndex].indices.empty()) {
            continue;
        }
        element val = rule.val;
        std::string indexFileName = tableName + "." + nt.attributes[attrIndex].name + ".index";
        std::set<long> newOffset;
        long tmpOffset;
        // B plus tree get offset according to filter rules
        // 传入文件名(indexFileName)，属性的值(val)
        switch (rule.type) {
            case 0: // <
                newOffset = getLessOffset(indexFileName, val);
                break;
            case 1: // <=
                newOffset = getLessOffset(indexFileName, val);
                tmpOffset = getOffset(indexFileName, val);
                if (tmpOffset != -1) {
                    newOffset.insert(tmpOffset);
                }
                break;
            case 2: // =
                assert(false);
                break;
            case 3: // >=
                newOffset = getMoreOffset(indexFileName, val);
                tmpOffset = getOffset(indexFileName, val);
                if (tmpOffset != -1) {
                    newOffset.insert(tmpOffset);
                }
                break;
            case 4: // >
                newOffset = getMoreOffset(indexFileName, val);
                break;
            case 5: // <>
                break;
            default: 
                assert(false);
                break;
        }
        std::vector<long> tmp(offset.size());
        std::vector<long>::iterator end = std::set_intersection(offset.begin(), offset.end(), newOffset.begin(), newOffset.end(), tmp.begin());
        offset.clear();
        for (std::vector<long>::iterator it = tmp.begin(); it != end; ++ it) {
            offset.insert(*it);
        }
    }
    
    for (auto x : offset) {
        // Record Manager delete records by using index
        // 传入数据库文件名（dbName），偏移量(x)，过滤器(filter)，当前表(nt)
        rm.RecordManagerRecordDelete(dbName, x, filter, nt);
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
    // 传入数据库文件名(dbName)，插入数据(entry)，当前表格(nt)
    rm.RecordManagerRecordInsert(dbName, entry, nt);
    // NOTE: table information may be changed after that
    return Response();
}

std::set<long> API::getMoreOffset(const std::string indexName, const element val) { // >
    return im.greater(indexName, val);
}

std::set<long> API::getLessOffset(const std::string indexName, const element val) { // <
    return im.less(indexName, val);
}

long API::getOffset(const std::string indexName, const element val) { // =
    return im.find(indexName, val);
}

//#define LOCAL_TEST
#ifdef LOCAL_TEST

int main() {
    return 0;
}
#endif
