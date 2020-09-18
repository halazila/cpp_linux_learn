#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace std;

// boost::asio::io_service io_service;
// boost::asio::ip::tcp::resolver resolver(io_service);
// boost::asio::ip::tcp::socket sock(io_service);
// boost::array<char, 4096> buffer;

// void read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
// {
//     if (!ec)
//     {
//         cout << std::string(buffer.data(), bytes_transferred) << endl;
//         sock.async_read_some(boost::asio::buffer(buffer), read_handler);
//     }
// }

// void connect_handler(const boost::system::error_code &ec)
// {
//     if (!ec)
//     {
//         boost::asio::write(sock, boost::asio::buffer("GET / HTTP 1.1\r\nHost: baidu.com\r\n\r\n"));
//         sock.async_read_some(boost::asio::buffer(buffer), read_handler);
//     }
// }

// void resolve_handler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator it)
// {
//     if (!ec)
//     {
//         sock.async_connect(*it, connect_handler);
//     }
// }

// int main(int argc, char *argv[])
// {
//     boost::asio::ip::tcp::resolver::query query("www.baidu.com", "80");
//     resolver.async_resolve(query, resolve_handler);
//     io_service.run();
//     return 0;
// }

// void handler1(const boost::system::error_code &ec)
// {
//     std::cout << "5 s." << std::endl;
// }

// void handler2(const boost::system::error_code &ec)
// {
//     std::cout << "10 s." << std::endl;
// }

// int main(int argc, char *argv[])
// {
//     boost::asio::io_service io_service;
//     boost::asio::deadline_timer timer2(io_service, boost::posix_time::seconds(10));
//     timer2.async_wait(handler2);
//     boost::asio::deadline_timer timer1(io_service, boost::posix_time::seconds(5));
//     timer1.async_wait(handler1);
//     io_service.run();
//     return 0;
// }

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <memory>
#include <functional>
// #include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_array.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>

bool compare(int i, int j)
{
    return i > j;
}
struct shmdtest
{
    int cnt;
    char buffer[128];
};

std::ostream &operator<<(std::ostream &out, const shmdtest &s)
{
    out << "cnt:" << s.cnt << "; buffer:" << s.buffer;
    return out;
}

int main(int argc, char *argv[])
{
    boost::scoped_ptr<int> sptr(new int(10));
    std::unique_ptr<int> uptr(new int(11));
    std::cout << *sptr << ";" << *uptr << std::endl;

    std::vector<int> vecint, vecint2;
    srand(10);
    for (int i = 0; i < 100; i++)
    {
        vecint.push_back(rand() % 100);
    }
    vecint2 = vecint;
    sort(vecint.begin(), vecint.end(), compare);
    sort(vecint2.begin(), vecint2.end(), std::bind(compare, std::placeholders::_2, std::placeholders::_1));
    std::cout << "vecint: ";
    for (size_t i = 0; i < vecint.size(); i++)
    {
        std::cout << vecint[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "vecint2: ";
    for (size_t i = 0; i < vecint2.size(); i++)
    {
        std::cout << vecint2[i] << ",";
    }
    std::cout << std::endl;

    std::cout << "/********boost::array**********/" << std::endl;
    boost::array<int, 10> arr1;
    arr1[0] = 11;
    std::cout << "arr1[0]=" << arr1[0] << std::endl;

    std::cout << "/********boost::shared_array**********/" << std::endl;
    boost::shared_array<int> sha_arr1;
    sha_arr1.reset(new int[10]);
    boost::shared_array<int> sha_arr2(sha_arr1);
    sha_arr1[0] = 100;
    std::cout << "sha_arr1[0]=" << sha_arr1[0] << ";sha_arr2[0]=" << sha_arr2[0] << std::endl;

    std::cout << "/********boost::ptr_vector**********/" << std::endl;
    boost::ptr_vector<int> ptr_vec;
    ptr_vec.push_back(new int(1));
    ptr_vec.push_back(new int(2));
    std::cout << "ptr_vec[0]=" << ptr_vec[0] << ";ptr_vec[1]=" << ptr_vec[1] << std::endl;

    std::cout << "/********boost::interprocess::shared_memory_object**********/" << std::endl;
    boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create, "boost_shm", boost::interprocess::read_write);
    shdmem.truncate(1024);
    boost::interprocess::mapped_region wregion(shdmem, boost::interprocess::read_write);
    shmdtest *wPtrShmtest = static_cast<shmdtest *>(wregion.get_address());
    wPtrShmtest->cnt = 190;
    sprintf(wPtrShmtest->buffer, "char buffer");
    boost::interprocess::mapped_region rregion(shdmem, boost::interprocess::read_only);
    shmdtest *rPtrShmtest = static_cast<shmdtest *>(rregion.get_address());
    std::cout << *rPtrShmtest << std::endl;
    boost::interprocess::shared_memory_object::remove("boost_shm");

    std::cout << "/********boost::bimap**********/" << std::endl;
    typedef boost::bimap<std::string, int> bimap;
    bimap perBimap;
    perBimap.insert(bimap::value_type("Boris", 31));
    perBimap.insert(bimap::value_type("Anton", 31));
    perBimap.insert(bimap::value_type("Caesar", 15));
    perBimap.insert(bimap::value_type("Boris", 21));
    for (auto it = perBimap.begin(); it != perBimap.end(); it++)
    {
        std::cout << it->left << " is " << it->right << " years old." << std::endl;
    }
    std::cout << perBimap.left.count("Boris") << std::endl;
    std::cout << perBimap.right.count(31) << std::endl;

    typedef boost::bimap<boost::bimaps::multiset_of<std::string>, boost::bimaps::multiset_of<int>> bmultimap;
    bmultimap perMultiBmap;
    perMultiBmap.insert(bimap::value_type("Boris", 31));
    perMultiBmap.insert(bimap::value_type("Anton", 31));
    perMultiBmap.insert(bimap::value_type("Caesar", 15));
    perMultiBmap.insert(bimap::value_type("Boris", 21));
    for (auto it = perMultiBmap.begin(); it != perMultiBmap.end(); it++)
    {
        std::cout << it->left << " is " << it->right << " years old." << std::endl;
    }
    std::cout << perMultiBmap.left.count("Boris") << std::endl;
    std::cout << perMultiBmap.right.count(31) << std::endl;
    return 0;
};