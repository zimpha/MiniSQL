#include "bufferManager.h"

bufferIter BFM::BufferManagerRead(const string &fileName, long offset)
{
	map <tag, bufferIter>::iterator it = table.find(make_pair(fileName,offset));
	if (it!=table.end()){ //already in the buffer
        Block tmp = *(it->second);
        buffer.erase(it->second); //move to the back of the buffer list
        buffer.push_back(tmp);
        bufferIter B = buffer.end();
        B--;
		return B;
	}
	else{ //not in the buffer
        FILE *inFile = fopen(fileName.c_str(), "rb");
        fseek(inFile, offset, SEEK_SET);
        Block tmp;
        fread(&tmp, 1, BLOCKSIZE, inFile);
        buffer.push_back(tmp); //push into buffer

        tag T = make_pair(tmp.fileName, tmp.offset);
        bufferIter B = buffer.end();
        B--;
        table.insert(make_pair(T,B)); //update table

        if (buffer.size()>BUFFERSIZE){ //we should use LRU replace
            bufferIter victim = buffer.begin();
            while (victim->status==1) victim++; //find the least used unpin block
            BufferManagerWrite(*victim);
            buffer.erase(victim);
        }

        return B;
	}
}

void BFM::BufferManagerPin(const Block &b)
{

}

void BFM::BufferManagerWrite(const Block &b)
{

}

int BFM::BufferManagerGetStatus(const Block &b)
{
    return 0;
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
