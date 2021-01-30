#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <sys/time.h>

using namespace std;

//stock code constructed with character '0~9' in fixed length of 6
struct StockPrefixNode
{
    StockPrefixNode *children[10];
    char *realVal; //only leaf node has realval
    StockPrefixNode() : realVal(nullptr)
    {
        std::fill(std::begin(children), std::end(children), nullptr);
    }
    ~StockPrefixNode()
    {
        if (realVal)
            delete realVal;
        for (auto it : children)
        {
            if (it)
                delete it;
        }
    }
};

struct StockPrefixTree
{
    StockPrefixNode *head;
    StockPrefixTree()
    {
        head = new StockPrefixNode;
    }
    ~StockPrefixTree()
    {
        delete head;
    }
    bool find(const char *code) //stock code, as: 600519
    {
        const char *p = code;
        StockPrefixNode *node = head;
        int k;
        while ( *p != '\0' && node->children[k = *p - '0'])
        {
            node = node->children[k];
            p++;
        }
        return *p == '\0' && node->realVal != nullptr; //stock code end && leaf node
    }
    void insert(const char *code) //stock code, as: 600519
    {
        const char *p = code;
        StockPrefixNode *node = head;
        while (*p != '\0')
        {
            int k = *p - '0';
            if (node->children[k] == nullptr)
                node->children[k] = new StockPrefixNode;
            node = node->children[k];
            p++;
        }
        int slen = p - code;
        if (slen)
        {
            node->realVal = new char[slen];
            memcpy(node->realVal, code, slen);
        }
    }
};

//benchmark StockPrefixTree vs unordered map
string genStockCode();
int microsends(timeval *t)
{
    return t->tv_sec * 1000000 + t->tv_usec;
}

int g_count = 1000000;
int g_find = 10000;
StockPrefixTree gStockTree;
unordered_map<string, bool> gStockMap;
bool gBoolVec[10000000];

int main(int argc, char *argv[])
{
    std::fill(std::begin(gBoolVec), std::end(gBoolVec), false);
    for (int i = 0; i < g_count; i++)
    {
        string code = genStockCode();
        gStockTree.insert(code.c_str());
        gStockMap[code] = true;
        gBoolVec[atoi(code.c_str())] = true;
    }

    vector<string> codeVec;
    for (size_t i = 0; i < g_find; i++)
    {
        codeVec.push_back(genStockCode());
    }

    timeval start;
    timeval end;
    //unordered_map
    int nfind = 0;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < codeVec.size(); i++)
    {
        if (gStockMap.find(codeVec[i]) != gStockMap.end())
        {
            // cout << codeVec[i] << ",";
            nfind++;
        }
    }
    gettimeofday(&end, NULL);
    cout << endl;
    printf("unordered_map: %d code founded, %ld microseconds spend\n", nfind, microsends(&end) - microsends(&start));
    //StockPrefixTree
    nfind = 0;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < codeVec.size(); i++)
    {
        if (gStockTree.find(codeVec[i].c_str()))
        {
            // cout << codeVec[i] << ",";
            nfind++;
        }
    }
    gettimeofday(&end, NULL);
    printf("StockPrefixTree: %d code founded, %ld microseconds spend\n", nfind, microsends(&end) - microsends(&start));
    //atoi function
    nfind = 0;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < codeVec.size(); i++)
    {
        if (gBoolVec[atoi(codeVec[i].c_str())])
            nfind++;
    }
    gettimeofday(&end, NULL);
    printf("BoolVector: %d code founded, %ld microseconds spend\n", nfind, microsends(&end) - microsends(&start));
    return 0;
}

string genStockCode()
{
    static char alpha[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    string ans;
    for (int i = 0; i < 6; i++)
    {
        ans.push_back(alpha[rand() % 10]);
    }
    return std::move(ans);
}