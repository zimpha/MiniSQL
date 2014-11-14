#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H

#include <string>
#include "global.h"
#include "filter.h"

/*
The size of each record = entrySize + 1 byte (available:0,unavailable:1)

Use the last 4 byte in each record block to identify whether the block is the last:
If the block is the last, it is a long int identify the next available position.
Otherwise, it is BLOCKSIZE.
*/

class RecordManager{
public:
    RecordManager(BFM bm);
    void rmInsertRecord(std::string dbName, const std::vector<element> entry, const Table nt);

    std::vector<std::vector<element> > rmSelectWithIndex(std::string dbName, long offset, const Filter filter, Table nt);
    std::vector<std::vector<element> > rmSelectWithoutIndex(std::string dbName, const Filter filter, Table nt);

    void rmDeleteWithIndex(std::string dbName, long offset, const Filter filter, const Table nt);    
    void rmDeleteWithoutIndex(std::string dbName, const Filter filter, const Table nt);

    std::set<long> rmGetAllOffsets(std::string dbName);

    int getInt(Block &block, int startPos);
    float getFloat(Block &block, int startPos);
    std::string getString(Block &block, int startPos);
private:
    BFM bm;
};

#endif