#include <iostream>
#include <thread>
#include <functional>

using namespace std;

class mvtrivial
{
public:
    int val = 10;

public:
    mvtrivial()
    {
        cout << "default constructor" << endl;
    }
    mvtrivial(mvtrivial &&oth)
    {
        cout << "move constructor" << endl;
    }
    ~mvtrivial()
    {
        cout << "deconstructor" << endl;
    }
};

thread g_thread;

void func_test(const mvtrivial &mvobj, const string &str)
{
    cout << "in func_test" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << str << endl;
}

void func_str(const string &str)
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << str << endl;
    cout << (void *)str.c_str() << endl;
}

void func_newthread()
{
    string str("Hello Jean");
    cout << (void *)str.c_str() << endl;
    g_thread = thread(std::bind(&func_str, (str)));
    // g_thread = thread(func_str, std::move(str));
    // g_thread = thread(func_str, str);
}

int main(int argc, char *argv[])
{
    mvtrivial obj;
    string str("Helloworld");
    //////3 times move constructor called
    // thread thd(std::bind(&func_test, std::move(obj), std::move(str)));
    // thd.join();

    /////1 times move constructor called
    // auto func = std::bind(&func_test, std::move(obj), std::move(str));
    // func();

    /////0 times constructor called
    // func_test(std::move(obj), std::move(str));
    // cout << str << endl;

    func_newthread();
    g_thread.join();
    return 0;
}