//#include <cstring>
#include "RecordManager.h"
#include "bufferManager.h"


union INT{
    int intFormat;
    char charFormat[4];
};

union FLOAT{
    float floatFormat;
    char charFormat[4];
};


int RecordManager::getInt(Block &block, int startPos)
{
    INT info;
    info.charFormat[0] = block.data[startPos];
    info.charFormat[1] = block.data[startPos + 1];
    info.charFormat[2] = block.data[startPos + 2];
    info.charFormat[3] = block.data[startPos + 3];
    return info.intFormat;
}

void RecordManager::setInt(Block &block, int startPos, int num)
{
    INT info;
    info.intFormat = num;
    block.data[startPos] = info.charFormat[0];
    block.data[startPos + 1] = info.charFormat[1];
    block.data[startPos + 2] = info.charFormat[2];
    block.data[startPos + 3] = info.charFormat[3];
}

float RecordManager::getFloat(Block &block, int startPos)
{
    FLOAT info;
    info.charFormat[0] = block.data[startPos];
    info.charFormat[1] = block.data[startPos + 1];
    info.charFormat[2] = block.data[startPos + 2];
    info.charFormat[3] = block.data[startPos + 3];
    return info.floatFormat;
}

std::string RecordManager::getString(Block &block, int startPos)
{
    //TODO: space for the string = user input + 1 !!
    char* str = (char*)block.data + startPos;
    std::string theString(str, sizeof(str));
    return theString;
}

RecordManager::RecordManager(BFM & bm) :bm(bm)
{

}
std::set<long> rmGetAllOffsets(std::string dbName)
{
    
}

void RecordManager::RecordManagerTableCreate(std::string dbName)
{
    Block firstBlock;
    firstBlock.fileName = dbName;
    firstBlock.offset = 0;
    setInt(firstBlock, BLOCKSIZE - 4, 0);
    bm.BufferManagerWrite(firstBlock);
}

void RecordManager::RecordManagerTableDetete(std::string dbName)
{
    bm.deleteFile(dbName);
}

std::vector<std::vector<element> > RecordManager::RecordManagerRecordSelect(std::string dbName, long offset, const Filter filter, Table nt)
{

    bufferIter pBlock;
    Block block;
    std::vector<std::vector<element> > queryResut;
    std::vector<element> queryTuple;
    int tupleSize = nt.entrySize + 1;

    pBlock = bm.BufferManagerRead(dbName, offset);
    block = *pBlock;
    int endPoint = getInt(block, BLOCKSIZE - 4);
    int tuple;
    int startPos;
    for (tuple = 0; tuple <= endPoint - tupleSize; tuple += tupleSize) {
        if (block.data[tuple] == 'N')
            continue;
        startPos = tuple + 1;
        queryTuple.clear();
        for (auto attr : nt.attributes) {
            switch (attr.type) {
                case 0:
                    queryTuple.push_back(element(getInt(block, startPos)));
                    startPos += sizeof(int);
                    break;
                case 1:
                    queryTuple.push_back(element(getFloat(block, startPos)));
                    startPos += sizeof(float);
                    break;
                case 2:
                    queryTuple.push_back(element(getString(block, startPos)));
                    startPos += attr.length;//Include +1
                    break;
                default:
                    break;
            }
        }
        if (true == filter.test(queryTuple))
            queryResut.push_back(queryTuple);
    }
    return queryResut;
}

std::set<long> RecordManager::RecordManagerGetAllOffsets(std::string dbName)
{    
    bufferIter pBlock;
    Block block;
    int endPoint;
    std::set<long> allOffsets;
    long offset = 0;
    do {
        pBlock = bm.BufferManagerRead(dbName, offset);
        block = *pBlock;
        endPoint = getInt(block, BLOCKSIZE - 4);
        allOffsets.insert(offset);
    } while (endPoint==BLOCKSIZE)
    
}