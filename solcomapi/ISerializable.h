#include <string>
#include <string.h>
#include <memory>

class CByteArray
{
private:
    char *pCharArr;
    int nCapacity;
    int nEnd;
    int nBegin;

public:
    CByteArray(int nCap = 1400)
    {
        nCapacity = nCap > 1400 ? nCap : 1400;
        pCharArr = new char[nCapacity];
        nEnd = nBegin = 0;
    }
    CByteArray(const CByteArray &other) = delete;
    CByteArray &operator=(const CByteArray &other) = delete;
    CByteArray(CByteArray &&other)
    {
        pCharArr = other.pCharArr;
        other.pCharArr = nullptr;
        nCapacity = other.nCapacity;
        nEnd = other.nEnd;
        nBegin = other.nBegin;
    }
    ~CByteArray()
    {
        if (pCharArr)
        {
            delete[] pCharArr;
        }
    }
    char *CharArray()
    {
        return pCharArr;
    }
    int ArrayCapacity()
    {
        return nCapacity;
    }
    int ArrayBegin()
    {
        return nBegin;
    }
    int ArrayEnd()
    {
        return nEnd;
    }
    ///对单元素的操作
    template <class T>
    CByteArray &Append(const T &data)
    {
        memcpy(pCharArr + nEnd, &data, sizeof(T));
        nEnd += sizeof(T);
        return *this;
    }
    template <class T>
    CByteArray &HeadIntercept(T &data)
    {
        memcpy(&data, pCharArr + nBegin, sizeof(T));
        nBegin += sizeof(T);
        return *this;
    }
    template <class T>
    CByteArray &TailIntercept(T &data)
    {
        nEnd -= sizeof(T);
        memcpy(&data, pCharArr + nEnd, sizeof(T));
        return *this;
    }
    ///对数组的操作
    template <class T>
    CByteArray &Append(const T *pArray, int nSize)
    {
        memcpy(pCharArr + nEnd, pArray, sizeof(T) * nSize);
        nEnd += sizeof(T);
        return *this;
    }
    template <class T>
    CByteArray &HeadIntercept(T *pArray, int nSize)
    {
        memcpy(pArray, pCharArr + nBegin, sizeof(T) * nSize);
        nBegin += sizeof(T) * nSize;
        return *this;
    }
    template <class T>
    CByteArray &TailIntercept(T *pArray, int nSize)
    {
        nEnd -= sizeof(T) * nSize;
        memcpy(pArray, pCharArr + nEnd, sizeof(T));
        return *this;
    }
};

class ISerializable
{
public:
    ISerializable() {}
    virtual ~ISerializable() {}
    virtual void Serialize(CByteArray &dstByteArray) = 0;
    virtual void Deserialize(CByteArray &srcByteArray, ISerializable &dstObject) = 0;
    virtual int dataSize()
    {
        //size of non-static data member (member function not included),
        //attention to byte alignment
        return sizeof(*this);
    }
};
