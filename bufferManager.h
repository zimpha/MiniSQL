#ifndef _bufferManager_h_
#define _bufferManager_h_

#define BLOCKSIZE 4096
#define BUFFERSIZE 1024

#include <list>
#include <string>
#include <cstdio>
#include <map>

using namespace std;

/**
	该文件是MiniSQL中的缓冲区管理模块头文件.
	推荐采取的文件存储方式 :
		为每一张表建立一个文件.
		为每一个索引建立一个文件
	通过传入文件名到该头文件中定义的函数中的方式来读取某个表/索引中的特定块.
	注意, 读取到的块的开头地址(offset)都是4096的整数倍, 每个块的大小为4KB
**/

class BFM;

struct Block{
public:
	unsigned char data[BLOCKSIZE];
	//data part
private:
    friend BFM;
    string fileName;
	//which file's block
	long offset;
	//start address
	int status;
	//block status 0--normal 1-pin
};

typedef list<Block>::iterator bufferIter;
typedef pair <string, long> tag;
typedef map <tag, bufferIter>::iterator tableIter;

class BFM{
public:
	bufferIter BufferManagerRead(const string &fileName, long offset);
	/*
		输入：1.需要读取的文件名 2.起始地址(请确保为BLOCKSIZE的倍数)
		操作：首先检查该块是否在主存缓冲区中, 若不在则在缓冲区中添加该块.
		      如果需要的话, 会按LRU策略从缓冲区中移出其他块来腾出空间.
		返回：bufferIter迭代器. 如果对读取的BLOCK有修改操作的话, 请直接对这个迭代器指向的内容进行修改
	*/
	void BufferManagerPin(const Block &b);
	/*
		输入：需要锁定的缓冲区块
		操作：修改这个块的状态为: 不允许被写出.
		返回：无
	*/
	void BufferManagerWrite(const Block &b);
	/*
		输入：需要强制写出的缓冲区块.(可以将通过BufferManagerPin锁定的块写出)
		操作：将这个块的内容写入磁盘中
		返回：无
	*/
	int  BufferManagerGetStatus(const Block &b);
	/*
		输入：需要获得状态的缓冲区块
		操作：定位对应的块, 读取其状态
		返回：0--普通状态(可能被替换出去)
		      1--锁定状态(不会被替换出去, 可以被强制写出)
	*/
	void flush();
	/*
		将缓冲区中所有的块都强制写出.
	*/
private:
	list <Block> buffer;
	map <tag, bufferIter> table;
};

#endif
