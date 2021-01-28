#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <future>
#include <stdlib.h>
#include <unistd.h>

// using namespace std;
std::mutex mtx;
std::condition_variable cv;
std::queue<int> gqueue;
int gcount = 10;

int main(int argc, char *argv[])
{
    std::vector<std::thread> thvec;
    std::vector<std::future<int>> futvec;
    for (int i = 0; i < gcount; i++)
    {
        std::packaged_task<int()> task([i]() -> int {
            sleep(rand() % 10);
            mtx.lock();
            gqueue.push(i);
            std::cout << "produce: " << i << std::endl;
            mtx.unlock();
            cv.notify_one();
            return i;
        });
        futvec.push_back(task.get_future());
        thvec.push_back(std::thread(std::move(task)));
    }
    int k = gcount;
    std::packaged_task<int()> task([&k]() -> int {
        while (k)
        {
            std::unique_lock<std::mutex> lock(mtx);
            while (gqueue.empty())
                cv.wait(lock);
            std::cout << "consume: " << gqueue.front() << std::endl;
            gqueue.pop();
            k--;
        }
        return k;
    });
    std::future<int> f = task.get_future();
    std::thread th(std::move(task));
    sleep(10);
    for (int i = 0; i < gcount; i++)
    {
        std::cout << "produce future: " << futvec[i].get() << std::endl;
        thvec[i].join(); //or thvec[i].detach();
    }
    std::cout << "consume future: " << f.get() << std::endl;
    th.join(); //or th.detach();
    return 0;
}