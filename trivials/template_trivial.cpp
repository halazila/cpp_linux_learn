#include <stdlib.h>
#include <stdio.h>
#include <queue>

//templace class success
template <class T>
struct my_base
{
    T value;
    my_base()
    {
        printf("base struct constructor\n");
    }
};
template <class T>
struct der_base : my_base<T>
{
    der_base()
    {
        printf("derived struct constructor\n");
    }
};

//template class special
template <>
struct std::greater<std::pair<int, double>>
{
    bool operator()(const std::pair<int, double> &v1, const std::pair<int, double> &v2)
    {
        return v1.second > v2.second;
    }
};

int main(int argc, char *argv[])
{
    my_base<int> mb;
    der_base<int> db;

    printf("****************\n");
    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, std::greater<std::pair<int, double>>> pq;
    pq.push(std::pair<int, double>(1, 5.001));
    pq.push(std::pair<int, double>(2, 5.101));
    pq.push(std::pair<int, double>(3, 4.601));
    pq.push(std::pair<int, double>(4, 3.101));
    printf("pq top: %f\n", pq.top().second);

    return 0;
}