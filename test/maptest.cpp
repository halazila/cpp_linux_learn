#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    unordered_map<int, int> unmap;
    cout << unmap.max_load_factor();
    return 0;
}