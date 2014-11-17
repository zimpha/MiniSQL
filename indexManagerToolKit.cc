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

void writeContent(FILE *file, long offset, const char *p, int n) {
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
        throw "bad write -- indexManager.writeContent"
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

vector < pair <element, long> > getRecord(const Table &table, const AttrType &attr) {
    //TODO
}