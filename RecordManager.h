#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H

#include <string>
#include "global.h"
#include "filter.h"
#include "bufferManager.h"

/*
The size of each record = 1 byte (available:'Y',unavailable:'N') + entrySize

Use the last 4 byte in each record block to identify whether the block is the last:
If the block is the last, it is a long int identify the next available position.
Otherwise, it is BLOCKSIZE.
*/

class RecordManager{
public:
    RecordManager(BFM & bm);
    void RecordManagerTableCreate(std::string dbName);
    void RecordManagerTableDetete(std::string dbName);
    void RecordManagerRecordInsert(std::string dbName, const std::vector<element> & entry, Table & nt);
    std::vector<std::vector<element> > RecordManagerRecordSelect(std::string dbName, long offset, const Filter & filter, Table & nt);
    void RecordManagerRecordDelete(std::string dbName, long offset, const Filter & filter, Table & nt);
    std::set<long> RecordManagerGetAllOffsets(std::string dbName, Table & nt);

    BFM & bm;

    int getInt(Block &block, int startPos);
    void setInt(Block &block, int startPos, int num);
    float getFloat(Block &block, int startPos);
    void setFloat(Block &block, int startPos, float num);
    std::string getString(Block &block, int startPos);
    void setString(Block &block, int startPos, std::string str);
    void InputRecord(Block & block, int tuplePos, const std::vector<element> & entry, Table & nt);

};

#endif