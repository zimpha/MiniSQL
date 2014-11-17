##ifndef _INDEXMANAGERTOOLKIT_H
#define _INDEXMANAGERTOOLKIT_H

bool isFileExist(const string &fileName);

void writeContent(FILE *file, long offset, const char *p, int n);

void writeContent(FILE *file, const char *p, int n);

string getIndexFileName(const string &indexName);

string getCheckFileName(const Table &table, const AttrType &attr);

Response goodRes();

Response badRes(const string &info);

void touch(const string &fileName);

vector < pair <element, long> > getRecord(const Table &table, const AttrType &attr);

#endif