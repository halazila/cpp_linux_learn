#include <iostream>

using namespace std;

#define MACRO_FUNC(T, q) test_##T##_func(q)

void test_int_func(int q)
{
    cout << "in value: " << q << endl;
    q++;
    cout << "after incre value: " << q << endl;
}

void test_double_func(double q)
{
    cout << "in value: " << q << endl;
    q++;
    cout << "after incre value: " << q << endl;
}

template <class T>
void template_func(T t)
{
    cout << "output data" << t << endl;
    MACRO_FUNC(T, t);
}

int main(int argc, char *argv[])
{
    MACRO_FUNC(int, 5);        //OK
    MACRO_FUNC(double, 6.235); //OK
    // template_func<int>(5);        //Error
    // template_func<double>(9.021); //Error
    return 0;
}