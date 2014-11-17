#include "indexManager.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
using namespace std;

const int SZ = (BLOCKSIZE - sizeof(long) - sizeof(int) - sizeof(element)) / (sizeof(long) + sizeof(element));
const string info_index_exist = "Index already exist.";


struct Node {
    int n;
    long off;                   //this
    long ch[SZ];                //child
    element value[SZ];
    element e;
};

Node *buildTree(const vector < pair <element, long> > &v) {
    //TODO
}

Node *saveTree(Node *root) {
    //TODO
}

Response IndexManager::create(const string &indexName, const Table &table, const AttrType &attr) {
    Response res;
    if (check(table, attr)) {
        return badRes(info_index_exist);
    } else {
        Node *root = buildTree(getContent());
        saveTree(root, getIndexFileName(indexName));
        return goodRes();
    }
}

Reponse IndexManager::drop(const std::string &indexName) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) {
        return badRes("No such index.");
    } else {
        bfm.deleteFile(name);
        return goodRes();
    }
}

bool IndexManager::check(const Table &table, const AttrType &attr) {
    return isFileExist(getCheckFileName(table, attr));
}

Response IndexManager::insert(const string &indexName, const element &e, long offset) {
    //TODO
}

Response IndexManager::erase(const string &indexName, const element &e) {
    //TODO
}