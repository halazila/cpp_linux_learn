#include <vector>

//并查集
struct UnionFindSet
{
    int nSize;
    std::vector<int> vecFat;
    UnionFindSet(int n) : nSize(n)
    {
        vecFat.resize(n);
        for (int i = 0; i < n; vecFat[i] = i++)
            ;
    }
    int find(int u)
    {
        return vecFat[u] == u ? vecFat[u] : vecFat[u] = find(vecFat[u]);
    }
    void unionVal(int u, int v)
    {
        vecFat[find(v)] = find(u);
    }
};

int main(int argc, char *argv[])
{
    return 0;
}