#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

thread_local int g_count = 1;

mutex cout_mtx;

class trival
{
public:
    thread_local static int nstatic;

public:
    trival(/* args */) {}
    ~trival() {}
};

thread_local int trival::nstatic = 10;

void bunc(const string &name)
{
    trival::nstatic++;
    lock_guard<mutex> lock(cout_mtx);
    cout << name << ", trival::nstatic=" << trival::nstatic << endl;
}

void func(const string &name)
{
    g_count++;
    lock_guard<mutex> lock(cout_mtx);
    cout << name << ", g_count=" << g_count << endl;
}

int main(int argc, char *argv[])
{
    thread a(func, "a");
    thread b(func, "b");
    thread c(func, "c");
    thread d(bunc, "d");
    thread e(bunc, "e");
    thread f(bunc, "f");
    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "main"
             << ", g_count=" << g_count << endl;
        cout << "main"
             << ", trival::nstatic=" << g_count << endl;
    }
    a.join();
    b.join();
    c.join();
    d.join();
    e.join();
    f.join();

    return 0;
}