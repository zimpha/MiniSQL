#include <string>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <strstream>
#include <algorithm>
#include <numeric>
#include "global.h"
#include "CatalogManager.h"
#include "interpreter.h"

void Interpreter::process(std::string line) {
    static std::string cmd;
    for (size_t i = 0; i < line.length(); ++ i) {
        if (line[i] == ';') {
            std::string now(cmd);
            cmd = "";
            parse(now);
        }
        else cmd += line[i];
    }
}

void Interpreter::execfile(std::string filename) {
    std::ifstream fin((filename).c_str());
    if (!fin) std::cout << "file does not exist" << std::endl;
    else {
        std::string buf;
        while (getline(fin, buf)) {
            int len = buf.size();
            while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
                len --;
            }
            process(buf.substr(0, len));
        }
    }
}

element Interpreter::parseElement(std::string data) {
    if (data[0] == '\'') {
        std::string res;
        res = data.substr(1, data.length() - 2);
        return res;
    }
    if (data.find('.') != -1) {
        char *s;
        return strtof(data.c_str(), &s);
    }
    else {
        char *s;
        return int(strtod(data.c_str(), &s) + 0.5);
    }
}

void Interpreter::parse(std::string input) {
    if (input == "quit") {
        exit(0);
    }
    if (input == "") return;
    
    for (size_t i = 0; i < input.length(); ++ i) {
        if (input[i] == '(' || input[i] == ')' || input[i] == '.') {
            input.insert(i, " ");
            input.insert(i + 2, " ");
            i += 2;
        }
        std::strstream sin;
        sin << input;
        
        std::string oper, null;
        sin >> oper;
        
        if (oper == "execfile") {
            std::string filename;
            sin >> filename;
            execfile(filename);
        }
        else if (oper == "create") {
            std::string type;
            sin >> type;
            
            if (type == "table") {
                std::string tableName;
                sin >> tableName >> null;
                // null == "("
                
                std::vector<AttrType> data;
                int pk = 0x3f3f3f3f;
                
                while (1) {
                    std::string attrName;
                    sin >> attrName;
                    
                    if (attrName == "primary") {
                        sin >> null; // null == "key"
                        sin >> null; // null == "("
                        std::string pkName;
                        sin >> pkName;
                        
                        for (size_t i = 0; i < data.size(); ++ i) {
                            if (data[i].name == pkName) {
                                pk = i;
                                break;
                            }
                        }
                        break;
                    }
                    
                    std::string dataType;
                    int length = 0;
                    sin >> dataType;
                    
                    if (dataType == "char") { // get char length
                        sin >> null; // null == "("
                        sin >> length;
                        sin >> null; // null == ")"
                    }
                    std::string unique;
                    sin >> unique;
                    
                    AttrType newAttr;
                    if (unique == "unique") {
                        newAttr.unique = true;
                        sin >> null; // null: ","
                    }
                    else {
                        // unique == ","
                        newAttr.unique = false;
                    }
                    newAttr.name = attrName;
                    if (dataType == "char") {
                        newAttr.type = 2;
                        newAttr.length = length + 1;
                    }
                    else if (dataType == "int") {
                        newAttr.type = 0;
                        newAttr.length = 0;
                    }
                    else if (dataType == "float") {
                        newAttr.type = 1;
                        newAttr.length = 0;
                    }
                    else {
                        std::cout << "unrecognized data type: " << dataType << std::endl;
                        return;
                    }
                    data.push_back(newAttr);
                }
                if (pk == 0x3f3f3f3f) {
                    std::cout << "don't set primary key or no such attribute" << std::endl;
                    return;
                }
                else {
                    Response res = api.createTable(tableName, data, pk);
                    if (!res.succeed) {
                        std::cout << res.info << std::endl;
                    }
                    else {
                        std::cout << "OK" << std::endl;
                    }
                }
            }
            else if (type == "index") {
                std::string indexName, tableName, attrName;
                sin >> indexName >> null >> tableName >> null >> attrName;
                // first null == "on"
                // second null == "("
                Response res = api.createIndex(indexName, tableName, attrName);
                if (!res.succeed) {
                    std::cout << res.info << std::endl;
                }
                else {
                    std::cout << "OK" << std::endl;
                }
            }
            else {
                std::cout << "syntax error" << std::endl;
                return;
            }
        }
        else if (oper == "drop") {
            std::string type;
            sin >> type;
            if (type == "table") {
                std::string tableName;
                sin >> tableName;
                Response res = api.dropTable(tableName);
                if (!res.succeed) {
                    std::cout << res.info << std::endl;
                }
                else {
                    std::cout << "OK" << std::endl;
                }
            }
            else if (type == "index") {
                std::string indexName;
                sin >> indexName;
                Response res = api.dropIndex(indexName);
                if (!res.succeed) {
                    std::cout << res.info << std::endl;
                }
                else {
                    std::cout << "OK" << std::endl;
                }
            }
            else {
                std::cout << "syntax error" << std::endl;
                return;
            }
        }
        else if (oper == "insert") {
            std::string tableName;
            sin >> null >> tableName >> null >> null;
            // first null == "into"
            // second null == "values"
            // third null == "("
            std::vector<element> entry;
            while (1) {
                std::string data;
                sin >> data;
                entry.push_back(parseElement(data));
                sin >> null; // null == ","
                if (null == ")") {
                    break;
                }
            }
            Response res = api.Insert(tableName, entry);
            if (!res.succeed) {
                std::cout << res.info << std::endl;
            }
            else {
                std::cout << "OK" << std::endl;
            }
        }
        else if (oper == "select") {
            std::string tableName;
            sin >> null >> null >> tableName;
            // first null == "*"
            // second null == "from"
            Filter filter;
            if (!api.cm.hasTable(tableName + ".table")) {
                std::cout << "table does not exist" << std::endl;
                return;
            }
            Table nt = api.cm.loadTable(tableName + ".table");
            while (sin >> null) { // null == "where", null == "and"
                std::string attrName, ope, data;
                sin >> attrName >> ope >> data;
                int index = -1, op;
                for (size_t i = 0; i < nt.attributes.size(); ++ i) {
                    if (nt.attributes[i].name == attrName) {
                        index = i;
                        break;
                    }
                }
                if (index == -1) {
                    std::cout << "attribute does not exist" << std::endl;
                    return;
                }
                if (ope == "<") op = 0;
                else if (ope == "<=") op = 1;
                else if (ope == "=") op = 2;
                else if (ope == ">=") op = 3;
                else if (ope == ">") op = 4;
                else if (ope == "<>") op = 5;
                else {
                    std::cout << "syntax error" << std::endl;
                    return;
                }
                filter.addRule(Rule(index, op, parseElement(data)));
            }
            Response res = api.Select(tableName, filter);
            if (!res.succeed) {
                std::cout << res.info << std::endl;
            }
            else {
                printSelectResult(nt, res);
            }
        }
        else if (oper == "delete") {
            std::string tableName;
            sin >> null >> tableName;// null == from;
            Filter filter;
            if (!api.cm.hasTable(tableName + ".table")) {
                std::cout << "table does not exist" << std::endl;
                return;
            }
            Table nt = api.cm.loadTable(tableName + ".table");
            while (sin >> null) { // null == "where", null == "and"
                std::string attrName, ope, data;
                sin >> attrName >> ope >> data;
                int index = -1, op;
                for (size_t i = 0; i < nt.attributes.size(); ++ i) {
                    if (nt.attributes[i].name == attrName) {
                        index = i;
                        break;
                    }
                }
                if (index == -1) {
                    std::cout << "attribute does not exist" << std::endl;
                    return;
                }
                if (ope == "<") op = 0;
                else if (ope == "<=") op = 1;
                else if (ope == "=") op = 2;
                else if (ope == ">=") op = 3;
                else if (ope == ">") op = 4;
                else if (ope == "<>") op = 5;
                else {
                    std::cout << "syntax error" << std::endl;
                    return;
                }
                filter.addRule(Rule(index, op, parseElement(data)));
            }
            Response res = api.Delete(tableName, filter);
            if (!res.succeed) {
                std::cout << res.info << std::endl;
            }
            else {
                std::cout << "delete OK" << std::endl;
            }
        }
        else {
            std::cout << "syntax error" << std::endl;
            return;
        }
    }
}

void Interpreter::printSelectResult(const Table &nt, const Response &res) {
    std::vector<int> space(nt.attributes.size());
    for (size_t i = 0; i < space.size(); ++ i) {
        space[i] = nt.attributes[i].name.length() + 1;
    }
    for (size_t i = 0; i < res.result.size(); ++ i) {
        for (size_t j = 0; j < res.result[i].size(); ++ j) {
            std::strstream sin;
            switch (res.result[i][j].type) {
                case 0:
                    sin << res.result[i][j].i;
                    break;
                case 1:
                    sin << res.result[i][j].f;
                    break;
                case 2:
                    sin << res.result[i][j].s;
                    break;
                default: assert(false);
            }
            std::string s; sin >> s;
            space[j] = std::max(space[j], (int)s.length() + 1);
        }
    }
    int sum = std::accumulate(space.begin(), space.end(), space.size() + 1);
    for (int _ = 0; _ < sum; ++ _) std::cout << "-"; 
    std::cout << std::endl;
    std::cout << "|";
    for (size_t i = 0; i < nt.attributes.size(); ++ i) {
        std::cout << nt.attributes[i].name;
        int extra = space[i] - nt.attributes[i].name.length();
        for (int _ = 0; _ < extra; ++ _) std::cout << " ";
        std::cout << "|";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < res.result.size(); ++ i) {
        std::cout << "|";
        for (size_t j = 0; j < res.result[i].size(); ++ j) {
            std::strstream sin;
            switch (res.result[i][j].type) {
                case 0:
                    sin << res.result[i][j].i;
                    break;
                case 1:
                    sin << res.result[i][j].f;
                    break;
                case 2:
                    sin << res.result[i][j].s;
                    break;
                default: assert(false);
            }
            std::string s; sin >> s;
            int extra = space[j] - s.length();
            res.result[i][j].print();
            for (int _ = 0; _ < extra; ++ _) std::cout << " ";
            std::cout << "|";
        }
        std::cout << std::endl;
    }
    for (int _ = 0; _ < sum; ++ _) std::cout << "-";
    std::cout << std::endl;
}
