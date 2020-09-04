#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

using namespace std;

struct risk_factor
{
    int factor_id;
    int level;
    int lnParam[5];
    int status;
    risk_factor()
    {
        factor_id = 0;
        level = 0;
        memset(lnParam, 0, sizeof(lnParam));
        status = 0;
    }
    friend ostream &operator<<(ostream &out, risk_factor &risk);
};

ostream &operator<<(ostream &out, risk_factor &risk)
{
    char tmp[128];
    memset(tmp, 0, 128);
    sprintf(tmp, "factor_id=%d,level=%d,lnParam={%d,%d,%d,%d,%d},status=%d", risk.factor_id, risk.level, risk.lnParam[0], risk.lnParam[1], risk.lnParam[2], risk.lnParam[3], risk.lnParam[4], risk.status);
    out << tmp;
}

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

void setRiskFactor(risk_factor &risk, string &sName, string &sVal)
{
    if (sName == "factor_id")
    {
        risk.factor_id = atoi(sVal.c_str());
    }
    else if (sName == "level")
    {
        risk.level = atoi(sVal.c_str());
    }
    else if (sName == "lnParam")
    {
        int i = 0;
        size_t it, pleft = 0;
        while ((it = sVal.find_first_of(',', pleft)) != string::npos)
        {
            risk.lnParam[i] = atoi(sVal.substr(pleft, it).c_str());
            pleft = it + 1;
            i++;
        }
        risk.lnParam[i] = atoi(sVal.substr(pleft).c_str());
    }
    else if (sName == "status")
    {
        risk.status = atoi(sVal.c_str());
    }
}

int main(int argc, char *argv[])
{
    ifstream ifs("risk_factor");
    string str;
    vector<risk_factor> riskFacVec;
    int i = -1;
    while (getline(ifs, str))
    {
        auto it = str.find_first_of('/');
        if (it == 0)
        {
            continue;
        }
        if (it != string::npos)
        {
            str.erase(str.begin() + it, str.end());
        }
        trim(str);
        if (str[0] == '[' && str[str.size() - 1] == ']')
        {
            riskFacVec.push_back(risk_factor());
            i++;
            continue;
        }
        string sName, sVal;
        it = str.find_first_of('=');
        if (it != string::npos)
        {
            sName = str.substr(0, it);
            sVal = str.substr(it + 1);
        }
        else
        {
            sName = str;
            sVal = "";
        }
        setRiskFactor(riskFacVec[i], sName, sVal);
    }
    for (int i = 0; i < riskFacVec.size(); i++)
    {
        cout << riskFacVec[i] << endl;
    }

    return 0;
}