#pragma once

#include <string.h>

///////////////////////////////////////
///类似于滑动窗口数组，提供左/右双指针操作
///数据有效区间[nLeft, nRight)，(左闭右开)
///默认数组长度为1400，支持扩容(手动操作)
///指针操作函数内部未做校验，使用时外部校验
///////////////////////////////////////
class CByteArray
{
private:
    char *pCharArr;
    int nCapacity;
    int nRight;
    int nLeft;

public:
    CByteArray(int nCap = 1400)
    {
        nCapacity = nCap > 1400 ? nCap : 1400;
        pCharArr = new char[nCapacity];
        nRight = nLeft = 0;
    }
    CByteArray(const CByteArray &other) = delete;
    CByteArray &operator=(const CByteArray &other) = delete;
    CByteArray(CByteArray &&other)
    {
        pCharArr = other.pCharArr;
        other.pCharArr = nullptr;
        nCapacity = other.nCapacity;
        nRight = other.nRight;
        nLeft = other.nLeft;
    }
    ~CByteArray()
    {
        if (pCharArr)
        {
            delete[] pCharArr;
        }
    }
    //初始化，左、右指针置为初始位置0
    void Init()
    {
        nLeft = nRight = 0;
    }
    //获取属性：内存位置
    char *CharArray()
    {
        return pCharArr;
    }
    //获取属性：内存容量（字节数）
    int Capacity()
    {
        return nCapacity;
    }
    //获取属性：左指针
    int LeftPos()
    {
        return nLeft;
    }
    //获取属性：右指针
    int RightPos()
    {
        return nRight;
    }
    //重分配存储空间
    void Reallocate(int newCap)
    {
        if (newCap > nCapacity)
        {
            char *pNewArr = new char[newCap];
            memcpy(pNewArr + nLeft, pCharArr + nLeft, nRight - nLeft);
            delete[] pCharArr;
            pCharArr = pNewArr;
        }
    }
    //左指针前移
    void LeftForward(int nSize)
    {
        nLeft += nSize;
    }
    //左指针后移
    void LeftBackward(int nSize)
    {
        nLeft -= nSize;
    }
    //右指针前移
    void RightForward(int nSize)
    {
        nRight += nSize;
    }
    //右指针后移
    void RightBackward(int nSzie)
    {
        nRight -= nSzie;
    }
    //尾部追加，右指针右移
    template <class T>
    CByteArray &Append(const T &data)
    {
        memcpy(pCharArr + nRight, &data, sizeof(T));
        nRight += sizeof(T);
        return *this;
    }
    template <class T>
    CByteArray &Append(const T *pArray, int nSize)
    {
        memcpy(pCharArr + nRight, pArray, sizeof(T) * nSize);
        nRight += sizeof(T) * nSize;
        return *this;
    }
    //头部追加，左指针左移
    template <class T>
    CByteArray &Prepend(const T &data)
    {
        nLeft -= sizeof(T);
        memcpy(pCharArr + nLeft, &data, sizeof(T));
        return *this;
    }
    template <class T>
    CByteArray &Prepend(const T *pArray, int nSize)
    {
        nLeft -= sizeof(T) * nSize;
        memcpy(pCharArr + nLeft, pArray, sizeof(T) * nSize);
        return *this;
    }
    //头部截取，左指针右移
    template <class T>
    CByteArray &HeadIntercept(T &data)
    {
        memcpy(&data, pCharArr + nLeft, sizeof(T));
        nLeft += sizeof(T);
        return *this;
    }
    template <class T>
    CByteArray &HeadIntercept(T *pArray, int nSize)
    {
        memcpy(pArray, pCharArr + nLeft, sizeof(T) * nSize);
        nLeft += sizeof(T) * nSize;
        return *this;
    }
    //尾部截取，右指针左移
    template <class T>
    CByteArray &TailIntercept(T &data)
    {
        nRight -= sizeof(T);
        memcpy(&data, pCharArr + nRight, sizeof(T));
        return *this;
    }
    template <class T>
    CByteArray &TailIntercept(T *pArray, int nSize)
    {
        nRight -= sizeof(T) * nSize;
        memcpy(pArray, pCharArr + nRight, sizeof(T));
        return *this;
    }
};
