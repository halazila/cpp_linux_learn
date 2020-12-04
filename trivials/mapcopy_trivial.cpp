#include <iostream>
#include <map>
#include <memory>
#include <vector>

class CpyTrivial
{
public:
    int value = 10;

public:
    CpyTrivial()
    {
        std::cout << "Constructor called" << std::endl;
    }
    CpyTrivial(const CpyTrivial &oth)
    {
        std::cout << "Copy Constructor called" << std::endl;
    }
    CpyTrivial(CpyTrivial &&oth)
    {
        std::cout << "Move Constructor called" << std::endl;
    }
    ~CpyTrivial()
    {
        std::cout << "Deconstructor called" << std::endl;
    }
    CpyTrivial &operator=(const CpyTrivial &oth)
    {
        std::cout << "Assign Constructor called" << std::endl;
        return *this;
    }
};

int main(int argc, char *argv[])
{
    std::map<int, CpyTrivial> testmap;
    for (int i = 0; i < 10; i++)
    {
        // testmap.insert(std::make_pair(i, CpyTrivial()));
        // testmap[i] = CpyTrivial();
        testmap[i];
    }
    std::cout << "*******************" << std::endl;
    std::map<int, CpyTrivial> copymap;
    copymap = testmap;
    std::cout << "*******************" << std::endl;
    std::map<int, CpyTrivial> assignmap(testmap);

    return 0;
}