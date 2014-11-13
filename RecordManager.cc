#include "RecordManager.h"
#include "bufferManager.h"

RecordManager::RecordManager(BFM bm) :bm(bm)
{

}
std::vector<std::vector<element> > RecordManager::rmSelectWithoutIndex(std::string dbName, const Filter filter, Table nt)
{
    bm.BufferManagerRead();
}