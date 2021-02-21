#include <stdlib.h>
#include <thread>
#include <sys/time.h>

using namespace std;

struct NorEntity
{
    volatile int64_t x;
};

//CPU cache line 64 bytes, fill remain space with unused data
struct PadEntity
{
    volatile int64_t x, p1, p2, p3, p4, p5, p6, p7;
};

NorEntity arrNor[2];
PadEntity arrPad[2];

void arrNorAddFunc(int n)
{
    for (int64_t i = 0; i < 100000000; i++)
    {
        arrNor[n].x = i;
    }
}

void arrPadAddFunc(int n)
{
    for (int64_t i = 0; i < 100000000; i++)
    {
        arrPad[n].x = i;
    }
}

int main(int argc, char *argv[])
{
    timeval tm_start, tm_end;
    //no-padding CPU cache line
    thread th1(arrNorAddFunc, 0);
    thread th2(arrNorAddFunc, 1);
    gettimeofday(&tm_start, NULL);
    th1.join();
    th2.join();
    gettimeofday(&tm_end, NULL);
    printf("No Padding spend %ld ms\n", (tm_end.tv_sec - tm_start.tv_sec) * 1000 + (tm_end.tv_usec - tm_start.tv_usec) / 1000);

    //padding CPU cache line
    thread th3(arrPadAddFunc, 0);
    thread th4(arrPadAddFunc, 1);
    gettimeofday(&tm_start, NULL);
    th3.join();
    th4.join();
    gettimeofday(&tm_end, NULL);

    printf("Padding spend %ld ms\n", (tm_end.tv_sec - tm_start.tv_sec) * 1000 + (tm_end.tv_usec - tm_start.tv_usec) / 1000);

    return 0;
}