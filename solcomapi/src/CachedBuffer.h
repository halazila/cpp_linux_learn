#pragma once

#include <memory>
#include <vector>
#include "ISerializable.h"

//单生产者单消费者--ringbuffer
template <class T>
class CachedBuffer
{
private:
    T *elements;
    int capacity; //2的整数次幂
    volatile uint64_t nHead;
    volatile uint64_t nCurrent;
    volatile uint64_t nTail;

public:
    CachedBuffer(int cap)
    {
        int n = 0;
        while (cap)
        {
            n++;
            cap >>= 1;
        }
        capacity = 1 << (n - 1);
        if (capacity < cap)
            capacity <<= 1;
        elements = new T[capacity];
        nHead = nCurrent = 0;
        nTail = -1;
    }
    ~CachedBuffer()
    {
        delete[] elements;
    }
    bool isEmpty()
    {
        return nTail + 1 == nHead;
    }
    bool isFull()
    {
        return nTail + 1 - capacity == nHead;
    }
    T *allocate()
    {
        if (isFull())
            return nullptr;
        return &elements[(nTail + 1) & (capacity - 1)];
    }
    bool headForward()
    {
        if (isEmpty())
            return false;
        nHead++;
        if (nCurrent < nHead)
            nCurrent = nHead;
        return true;
    }
    //move nCurrent position forward one step
    bool currentForward()
    {
        if (nCurrent > nTail)
            return false;
        nCurrent++;
        return true;
    }
    bool tailForward()
    {
        if (isFull())
            return false;
        nTail++;
        return true;
    }
    T *front()
    {
        if (isEmpty())
            return nullptr;
        return &elements[nHead & (capacity - 1)];
    }
    T *current()
    {
        if (isEmpty())
            return nullptr;
        return &elements[nCurrent & (capacity - 1)];
    }
    T *back()
    {
        if (isEmpty())
            return nullptr;
        return elements[nTail & (capacity - 1)];
    }
};
