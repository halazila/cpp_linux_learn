#include <iostream>
#include <chrono>
#include <unordered_map>

typedef std::chrono::system_clock myclock;

int main(int argc, char *argv)
{

    using clock = std::chrono::system_clock;
    myclock::time_point nowp = clock::now();
    myclock::time_point endp = nowp + std::chrono::minutes(5);
    time_t nowt = clock::to_time_t(nowp);
    time_t endt = clock::to_time_t(endp);
    std::cout << ctime(&nowt) << std::endl;
    std::cout << ctime(&endt) << std::endl;
    std::cout << (endp > nowp) << std::endl;

    int k = 10;
    int &p = k;
    auto q = p;
    q++;
    auto &r = p;
    r += 2;
    std::cout << "q = " << q << ", k = " << k << std::endl;
    std::unordered_map<int, int> nmap;
    nmap[0] = 0;
    nmap[1] = 1;
    auto mk = nmap[0];
    mk++;
    auto &mr = nmap[1];
    mr++;
    std::cout << "nmap[0] = " << nmap[0] << ", nmap[1] = " << nmap[1] << std::endl;

    return 0;
}
