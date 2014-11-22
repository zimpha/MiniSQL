#include "bufferManager.h"
#include <iostream>

using namespace std;

map <tag, bufferIter> BFM::table;
list <Block> BFM::buffer;

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
        if(inFile==NULL){
            cerr<<"ERROR: Open file "<<fileName<<" failed.\n";
        }
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
    FILE *outFile = fopen(b.fileName.c_str(), "rb+");
    if(outFile==NULL){
        cerr<<"ERROR: Open file "<<b.fileName<<" failed.\n";
        return;
    }
    fseek(outFile, b.offset, SEEK_SET);
    fwrite(&b.data, 1, BLOCKSIZE, outFile);
    fclose(outFile);

    tag T = make_pair(b.fileName, b.offset);
    tableIter tmp = table.find(T);
    if (tmp!=table.end()){ //in the buffer, which means not new written
        bufferIter victim = tmp->second;
        table.erase(table.find(T)); //remove from table
        buffer.erase(victim); //remove from buffer
    }

}

int BFM::BufferManagerGetStatus(const Block &b)
{
    return b.status;
}

void BFM::BufferManagerFlush()
{
    for (bufferIter T = buffer.begin(); T!=buffer.end();){
        BufferManagerWrite(*T++);
    }
    buffer.clear();
    table.clear();
}

void BFM::deleteFile(const string &fileName)
{
    tableIter tIt = table.lower_bound(make_pair(fileName,0));
    for (; tIt!=table.end() && tIt->first.first==fileName; ){
        BufferManagerWrite(*(tIt++->second));
    }
    remove(fileName.c_str());
}
