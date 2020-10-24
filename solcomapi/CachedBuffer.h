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
    volatile uint64_t head;
    volatile uint64_t current;
    volatile uint64_t tail;

public:
    CachedBuffer(int cap)
    {
        int n = 0;
        while (cap)
        {
            n++;
            cap >>= 1;
        }
        capacity = 1 << n;
        elements = new T[capacity];
        head = current = 0;
        tail = -1;
    }
    ~CachedBuffer()
    {
        delete[] elements;
    }
    bool isEmpty()
    {
        return tail + 1 == head;
    }
    bool isFull()
    {
        return tail + 1 - capacity == head;
    }
    T *allocate()
    {
        if (isFull())
            return nullptr;
        return &elements[(tail + 1) & (capacity - 1)];
    }
    bool headForward()
    {
        if (isEmpty())
            return false;
        head++;
        if (current < head)
            current = head;
        return true;
    }
    //move current position forward one step
    bool currentForward()
    {
        if (current > tail)
            return false;
        current++;
        return true;
    }
    bool tailForward()
    {
        if (isFull())
            return false;
        tail++;
        return true;
    }
    T *front()
    {
        if (isEmpty())
            return nullptr;
        return &elements[head & (capacity - 1)];
    }
    T *current()
    {
        if (isEmpty())
            return nullptr;
        return &elements[current & (capacity - 1)];
    }
    T *back()
    {
        if (isEmpty())
            return nullptr;
        return elements[tail & (capacity - 1)];
    }
};
