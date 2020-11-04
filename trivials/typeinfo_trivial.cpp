#include <iostream>

using namespace std;
class X
{
private:
    int mX;

public:
    X(/* args */) { mX = 101; }
    virtual void vfunc()
    {
        std::cout << "X::vfunc()" << endl;
    }
};

class XX : public X
{
private:
    int mXX;

public:
    XX() : X()
    {
        mXX = 1001;
    }
    virtual void vfunc()
    {
        std::cout << "XX::vfunc()" << endl;
    }
};

void printTypeInfo(const X *px)
{
    cout << "typeid(px) -> " << typeid(px).name() << endl;
    cout << "typeid(*px) -> " << typeid(*px).name() << endl;
}

typedef void (*FuncPtr)();

int main()
{
    X x;
    XX xx;
    //typeid(px) return same, static info
    //typeid(*px) return diff, dynamic info
    printTypeInfo(&x);
    printTypeInfo(&xx);

    FuncPtr func;
    char *p = (char *)&xx;
    //获取虚函数表的地址(vptr)
    int64_t **vtbl = (int64_t **)*(int64_t **)p;
    //输出虚函数表的地址, value of vptr
    cout << vtbl << endl;
    //获取type_info对象的指针，并调用name成员函数
    cout << "\t[-1]: " << (vtbl[-1]) << " -> " << ((type_info *)(vtbl[-1]))->name() << endl;
    //调用第一个virtual函数
    cout << "\t[0]: " << vtbl[0] << " -> ";
    func = (FuncPtr)vtbl[0];
    func();
    //输出基类的成员变量的值
    p += sizeof(int64_t **);
    cout << *(int *)p << endl;
    //输出派生类成员变量的值
    p += sizeof(int);
    cout << *(int *)p << endl;

    return 0;
}
