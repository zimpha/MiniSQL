#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H

#include <string>
#include "global.h"

class RecordManager{
public:
    void rmInsertRecord(std::string dbName, const std::vector<element> entry, const Table nt);

    std::vector<std::vector<element> > rmSelectWithIndex(std::string dbName, long offset, const Filter filter, Table nt);
    std::vector<std::vector<element> > rmSelectWithoutIndex(std::string dbName, const Filter filter, Table nt);

    void rmDeleteWithIndex(std::string dbName, long offset, const Filter filter, const Table nt);    
    void rmDeleteWithoutIndex(std::string dbName, const Filter filter, const Table nt);

    std::set<long> rmGetAllOffsets(std::string dbName);
};

#endif