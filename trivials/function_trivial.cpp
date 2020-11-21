#include <iostream>
#include <functional>

int main(int argc, char *argv[])
{
    std::function<void()> func;
    std::cout << (func ? "func callable" : "func not callable") << std::endl;
    std::function<int(int a)> bunc = [](int a) {
        a++;
        return a;
    };
    std::cout << (bunc ? "bunc callable" : "bunc not callable") << std::endl;
    return 0;
}
