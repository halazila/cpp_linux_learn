#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

int main(int argc, char *argv[])
{
    int nTotal = 700000;

    map<std::string, bool> srtMap;
    unordered_map<std::string, bool> uordMap;
    uordMap.reserve(65536);
    bool *bArr = new bool[nTotal];

    std::vector<std::string> strVector;
    strVector.reserve(nTotal);
    char tmp[7];

    for (int i = 0; i < nTotal; i++)
    {
        sprintf(tmp, "%06d", i);
        strVector.push_back(tmp);
    }

    srand(100);
    int k, t, nTimes = 60000;
    auto tStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nTimes; i++)
    {
        k = rand() % nTotal;
        t = atoi(strVector[k].c_str());
        bArr[t] = true;
    }
    auto tEnd = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart);
    std::cout << "atoi spend " << duration.count() << " us" << std::endl;
    ///////////////////////////////////////////////////////////////////////////////////////////
    tStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nTimes; i++)
    {
        k = rand() % nTotal;
        uordMap[strVector[k]] = true;
    }
    tEnd = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart);
    std::cout << "umap spend " << duration.count() << " us" << std::endl;
    ////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<int> nRandVec;
    for (int i = 0; i < nTimes; i++)
    {
        k = rand() % nTotal;
        nRandVec.push_back(k);
        srtMap[strVector[k]] = true;
    }
    tStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nTimes; i++)
    {
        k = rand() % nTimes;
        srtMap[strVector[nRandVec[k]]] = true;
    }
    tEnd = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart);
    std::cout << "smap spend " << duration.count() << " us" << std::endl;

    return 0;
}