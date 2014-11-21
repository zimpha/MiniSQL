#include "indexManager.h"
#include "RecordManager.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
using namespace std;

const int SZ = (BLOCKSIZE - sizeof(long) - sizeof(int) - sizeof(bool) * 2) / (sizeof(long) + sizeof(element)) - 1;
const std::string info_index_exist = "Index already exist.";


struct BTNode {
    bool isLeaf;
    bool isRoot;
    int n;                          //number of element in this node
    long offset;                    //this
    long ch[SZ + 1];                //child
    element value[SZ + 1];
    bool operator < (const BTNode &oth) {
        return offset < oth.offset;
    }
};

bool isFileExist(const string &fileName) {
    ifstream f(fileName.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

void writeContent(FILE *file, long offset, const void *p, int n) {
    if (!fseek(file, offset, SEEK_SET)) {
        if (fwrite((const void *)p, sizeof(char), n, file) != n) {
            throw "bad write -- indexManager.writeContent";
        }
    } else {
        throw "bad seek -- indexManager.writeContent";
    }
}

void writeContent(FILE *file, const char *p, int n) {
    if (fwrite((const void *)p, sizeof(char), n, file) != n) {
        throw "bad write -- indexManager.writeContent";
    }
}

string getIndexFileName(const string &indexName) {
    return indexName + ".index";
}

string getCheckFileName(const Table &table, const AttrType &attr) {
    return table.name + "_" + attr.name + ".check";
}

Response goodRes() {
    Response res;
    res.succeed = true;
    return res;
}

Response badRes(const string &info) {
    Response res;
    res.succeed = false;
    res.info = info;
    return res;
}

void touch(const string &fileName) {
    FILE *f = fopen(fileName.c_str(), "w");
    fclose(f);
}

vector < pair <element, long> > getRecord(BFM &bfm, const string &dbName, Table &table, const AttrType &attr) {
    RecordManager rm(bfm);
    set<long> s = rm.RecordManagerGetAllOffsets(dbName, table);
    Filter filter;
    vector < pair <element, long> > v;
    int idx = table.getIndexID(attr);
    if (idx == -1) return v;
    for (long offset : s) {
        vector < vector <element> > res = rm.RecordManagerRecordSelect(dbName, offset, filter, table);
        for (vector <element> &u : res) {
            v.push_back(make_pair(u[idx], offset));
        }
    }
    return v;
}

BTNode blockToNode(const Block &b) {
    return BTNode();
}

map <element, long> build(const vector < pair <element, long> > &v) {
    map <element, long> mp;
    for (auto o : v) 
        mp[o.first] = o.second;
    return mp;
}

namespace BT {
    BFM *bfm;
    string name;
    BTNode pool[50000], *C;

    BTNode newNode() {
        FILE *file = fopen(name.c_str(), "rb");
        fseek(file, 0, SEEK_END);
        long off = ftell(file);
        fclose(file);
        BTNode res;
        res.offset = off;
        return res;
    }

    void setRoot(long offset) {
        BFM bfm;
        bfm.BufferManagerFlush();
        FILE *file = fopen(name.c_str(), "r+b");
        writeContent(file, 0, &offset, sizeof(long));
        fclose(file);
    }

    BTNode *build(const vector < pair <element, long> > &v) {
        C = pool;
        C->n = 0;
        C->isLeaf = C->isRoot = 1;
        return C;
    }

    BTNode save(const BTNode &p) {
        bfm->BufferManagerFlush();
        FILE *file = fopen(name.c_str(), "r+b");
        writeContent(file, p.offset, &p, sizeof(p));
        fclose(file);
        return BTNode();
    }

    void insertParent(map <long, long> &parent, BTNode p, const element &e, BTNode q) {
        if (p.isRoot) {
            BTNode w = newNode();
            w.n = 1;
            w.isRoot = true;
            //setRoot(name, bfm, w.offset);
            w.value[0] = e;
            w.ch[0] = p.offset;
            w.ch[1] = q.offset;
            save(w);
            return;
        }
        auto go = [&] (long off) { return blockToNode(*(bfm->BufferManagerRead(name, off))); };
        BTNode f = go(parent[p.offset]);
        if (f.n < SZ - 1) {
            int i = 0;
            while (i < f.n && p.offset != f.ch[i]) i++;
            for (int j = f.n + 1; j > i + 1; j--) {
                f.ch[j] = f.ch[j - 1];
                f.value[j - 1] = f.value[j - 2];
            }
            f.ch[i + 1] = q.offset;
            f.value[i] = q.value[0];
            save(f);
        } else { //split
            vector <element> ele(f.value, f.value + f.n);
            vector <long> off(f.offset, f.offset + f.n + 1);
            int i = 0;
            while (off[i] != p.offset) i++;
            off.insert(off.begin() + i + 1, q.offset);
            ele.insert(ele.begin() + i, e);
            int m = (SZ + 1) / 2;
            BTNode w = newNode();
            f.n = m - 1;
            w.n = ele.size() - m;
            for (i = 0; i < m - 1; i++) {
                f.value[i] = ele[i];
                f.ch[i] = off[i];
            }
            f.ch[m - 1] = off[m - 1];
            element cur = ele[m - 1];
            for (i = m; i < ele.size(); i++) {
                w.value[i] = ele[i];
                w.ch[i] = off[i];
            }
            w.ch[w.n] = off.back();
            save(w);
            save(f);
            insertParent(parent, f, cur, w);
        }
    }

    void insert(const element &e, long offset) {
        auto go = [&] (long off) { return blockToNode(*(bfm->BufferManagerRead(name, off))); };
        BTNode c;//s
        map <long, long> parent;
        while (!c.isLeaf) {
            int i = 0;
            while (i < c.n && c.value[i] <= e) i++;
            parent[c.ch[i]] = c.offset;
            c = go(c.ch[i]);
        }
        if (c.n < SZ - 1) {
            //BT::insert_in_leaf(c, e, offset);
        } else { // split
            BTNode p = BT::newNode();
            vector < pair <element, long> > v;
            for (int i = 0; i < c.n; i++) v.push_back(make_pair(c.value[i], c.ch[i]));
            auto it = v.begin();
            while (it != v.end() && e < it->first) it++;
            v.insert(it, make_pair(e, offset));
            int m = (SZ + 1) / 2;
            p.ch[SZ] = c.ch[SZ];
            c.ch[SZ] = p.offset;
            c.n = m;
            p.n = v.size() - m;
            for (size_t i = 0; i < v.size(); i++) {
                (i < m ? c.value[i] : p.value[i - m]) = v[i].first;
                (i < m ? c.ch[i]    : p.ch[i - m]   ) = v[i].second;
            }
            BT::insertParent(parent, c, p.value[0], p);
        }
    }
}

IndexManager::IndexManager(BFM &bfm): bfm(bfm) {
    currentFile = "";
    mp.clear();
}

Response IndexManager::create(const string &indexName, const string &dbName, Table &table, const AttrType &attr) {
    Response res;
    if (isFileExist(getIndexFileName(indexName))) {
        return badRes(info_index_exist);
    } else {
        save();
        currentFile = getIndexFileName(indexName);
        mp = build(getRecord(bfm, dbName, table, attr));
        save();
        return goodRes();
    }
}

Response IndexManager::drop(const std::string &indexName) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) {
        return badRes("No such index. -- IndexManager::drop");
    } else {
        bfm.deleteFile(name);
        return goodRes();
    }
}

Response IndexManager::insert(const string &indexName, const element &e, long offset) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return badRes("No such Index. -- IndexManager::insert");
    if (name != currentFile) {
        save();
        currentFile = name;
        load();
    }
    if (mp.count(e)) return badRes("Element exist. -- IndexManager::insert");
    mp[e] = offset;
    return goodRes();
}

Response IndexManager::erase(const string &indexName, const element &e) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return badRes("No such Index. -- IndexManager::erase");
    if (name != currentFile) {
        save();
        currentFile = name;
        load();
    }
    if (!mp.count(e)) return badRes("No such element. -- IndexManager::erase");
    mp.erase(e);
    return goodRes();
}

long IndexManager::find(const std::string &indexName, const element &e) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return -1;
    if (!mp.count(e)) return -1;
    return mp[e];
}

set <long> IndexManager::greater(const std::string &indexName, const element &e) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return set <long> ();
    set <long> res;
    for (auto it = mp.upper_bound(e); it != mp.end(); it++) {
        res.insert(it->second);
    }
    return res;
}

set <long> IndexManager::less(const std::string &indexName, const element &e) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return set <long> ();
    set <long> res;
    for (auto it = mp.begin(); it != mp.end() && it->first < e; it++) {
        res.insert(it->second);
    }
    return res;
}

set <long> IndexManager::inRange(const std::string &indexName, const element &lhs, const element &rhs) {
    string name = getIndexFileName(indexName);
    if (!isFileExist(name)) return set <long> ();
    set <long> res;
    for (auto it = mp.lower_bound(lhs); it != mp.end() && it->first <= rhs; it++) {
        res.insert(it->second);
    }
    return res;
}

void IndexManager::save() {
    FILE *file = fopen(currentFile.c_str(), "wb");
    int tmp = mp.size();
    fwrite(&tmp, sizeof(int), 1, file);
    for (auto o : mp) {
        fwrite(&o.first, sizeof(o.first), 1, file);
        fwrite(&o.second, sizeof(o.second), 1, file);
    }
    fclose(file);
}

void IndexManager::load() {
    FILE *file = fopen(currentFile.c_str(), "rb");
    mp.clear();
    int n = 0;
    element lhs;
    long rhs;
    fread(&n, sizeof(int), 1, file);
    for (int i = 0; i < n; i++) {
        fread(&lhs, sizeof(lhs), 1, file);
        fread(&rhs, sizeof(rhs), 1, file);
        mp[lhs] = rhs;
    }
}