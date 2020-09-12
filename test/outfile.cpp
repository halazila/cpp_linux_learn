#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
    ofstream ofa, ofb;
    ofa.open("output.txt");
    ofb.open("output.txt");
    for (size_t i = 0; i < 100; i++)
    {
        ofa << "ofa line aaa" << i << endl; //ofs 写的内容被部分覆盖
        // ofa << "ofa line " << i << endl;    //ofs 写的内容被覆盖
        ofb << "ofb line " << i << endl;
    }
    ofa.close();
    ofb.close();
    ifstream ifs("output.txt");
    string str;
    while (getline(ifs, str))
    {
        cout << str << endl;
    }

    return 0;
}