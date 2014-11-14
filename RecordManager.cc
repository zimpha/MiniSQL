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
std::vector<std::vector<element> > RecordManager::rmSelectWithoutIndex(std::string dbName, const Filter filter, Table nt)
{
    long offset = 0;
    bufferIter pBlock;
    Block block;

    while (1) {
        pBlock = bm.BufferManagerRead(dbName, offset);
        block = *pBlock;
        int endPoint = getInt(block, BLOCKSIZE - 4);
        int tuple;
        int tupleSize = nt.entrySize + 1;
        for (tuple = 0; tuple <= endPoint - tupleSize; tuple += tupleSize) {
            for (auto attr : nt.attributes) {
                if (attr.type==)
                    //TODO
            }                
        }
        if (endPoint != BLOCKSIZE)
            break;
    }
    
}