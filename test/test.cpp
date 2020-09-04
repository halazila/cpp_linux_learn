#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

void ltrim(string &s)
{
    size_t i = 0;
    while (i < s.size() && isspace(s[i]))
    {
        i++;
    }
    s.erase(s.begin(), s.begin() + i);
}
void rtrim(string &s)
{
    size_t i = s.size() - 1;
    while (i >= 0 && isspace(s[i]))
    {
        i--;
    }
    s.erase(s.begin() + i + 1, s.end());
}
void trim(string &s)
{
    rtrim(s);
    ltrim(s);
}

int main(int argc, char *argv[])
{
    ifstream ifs("tmp.txt");
    if (ifs.is_open())
    {
        char buffer[512];
        string str, sec, sval;
        while (getline(ifs, str))
        {
            // cout << buffer << endl;
            // str = buffer;
            // cout << str << endl;
            auto it = str.find_first_of('=');
            if (it != string::npos)
            {
                sec = str.substr(0, it);
                sval = str.substr(it + 1);
            }
            else
            {
                sec = str;
                sval = "";
            }
            it = sval.find_first_of(';');
            if (it != string::npos)
            {
                sval.erase(sval.begin() + it, sval.end());
            }

            trim(sec);
            trim(sval);
            cout << "sec=" << sec << "; val=" << sval << endl;
        }
    }
    // cout << atoi("") << endl;
    // cout << atoi("fdh") << endl;
    // cout << atoi("21fd") << endl;
    // cout << atoi("fe21") << endl;
    // char *chbuf = (char *)"SH600213";
    // char *chnul = "";
    char arbuf[24];
    // strncpy(arbuf, chbuf + 2, 23);
    // cout << arbuf << endl;
    // strcpy(arbuf, chnul);
    // cout << arbuf << endl;

    // string s;
    // cout << "string default capacity: " << s.capacity() << endl;

    // printf("reta%ld", (long)1212121);

    // strcpy(arbuf, "10:51:21");
    // string ts(arbuf + 6);
    // cout << ts << endl;
    ifstream optionfile("./OptionsList");
    if (optionfile.is_open())
    {
        string strline;
        while (getline(optionfile, strline))
        {
            string tmp1, tmp2;
            istringstream ss(strline);
            while (getline(ss, tmp1, ' ') && tmp1.empty())
            {
                //cout << tmp1 << endl;
            }
            while (getline(ss, tmp2, ' ') && tmp2.empty())
            {
                // cout << tmp2 << endl;
            }
            cout << tmp1 << " " << tmp2 << endl;
        }
    }
    return 0;
}