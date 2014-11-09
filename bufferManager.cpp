#include "bufferManager.h"

bufferIter BFM::BufferManagerRead(const string &fileName, long offset)
{
	tableIter it = table.find(make_pair(fileName,offset));
	if (it!=table.end()){ //already in the buffer
        Block tmp = *(it->second);
        buffer.erase(it->second); //move to the back of the buffer list
        buffer.push_back(tmp);

        bufferIter B = buffer.end();
        B--;
        tag T = make_pair(tmp.fileName, tmp.offset);
        table.find(T)->second = B; //update table
		return B;
	}
	else{ //not in the buffer
        FILE *inFile = fopen(fileName.c_str(), "rb");
        fseek(inFile, offset, SEEK_SET);
        Block tmp;
        tmp.fileName = fileName;
        tmp.offset = offset;
        tmp.status = 0;
        fread(&tmp.data, 1, BLOCKSIZE, inFile);
        fclose(inFile);

        buffer.push_back(tmp); //push into buffer

        tag T = make_pair(tmp.fileName, tmp.offset);
        bufferIter B = buffer.end();
        B--;
        table.insert(make_pair(T,B)); //update table

        if (buffer.size()>BUFFERSIZE){ //we should use LRU replace
            bufferIter victim = buffer.begin();
            while (victim->status==1) victim++; //find the least used unpin block
            BufferManagerWrite(*victim); //writeback
        }

        return B;
	}
}

void BFM::BufferManagerPin(Block &b)
{
    b.status = 1;
}

void BFM::BufferManagerWrite(const Block &b)
{
    FILE *outFile = fopen(b.fileName.c_str(), "wb");
    fseek(outFile, b.offset, SEEK_SET);
    fwrite(&b.data, 1, BLOCKSIZE, outFile);
    fclose(outFile);

    tag T = make_pair(b.fileName, b.offset);
    tableIter tmp = table.find(T);
    bufferIter victim = tmp->second;
    table.erase(table.find(T)); //remove from table
    buffer.erase(victim); //remove from buffer
}

int BFM::BufferManagerGetStatus(const Block &b)
{
    return b.status;
}

void BFM::flush()
{
    for (bufferIter T = buffer.begin(); T!=buffer.end(); T++){
        BufferManagerWrite(*T);
    }
    buffer.clear();
    table.clear();
}

int main()
{
    return 0;
}
