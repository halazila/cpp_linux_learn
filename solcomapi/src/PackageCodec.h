#pragma once

#include "ISerializable.h"
#include "CommonStruct.h"

class PackageCodec
{
private:
    /* data */
public:
    PackageCodec() {}
    ~PackageCodec() {}
    void encodeHead(PackageHeadField &pkgHead, CByteArray &byteArr)
    {
        byteArr.Append(pkgHead);
    }
    PackageHeadField decodeHead(CByteArray &byteArr)
    {
        PackageHeadField pkgHead;
        byteArr.HeadIntercept(pkgHead);
        return pkgHead;
    }
    void encodeSerializeObj(ISerializable &objSerializable, CByteArray &byteArr)
    {
        objSerializable.Serialize(byteArr);
    }
    void decodeSerializeObj(ISerializable &objSerializable, CByteArray &byteArr)
    {
        objSerializable.Deserialize(byteArr);
    }
};