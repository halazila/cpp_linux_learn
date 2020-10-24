#include "ISerializable.h"

#pragma pack(push, 4)
struct PackageHeadField
{
    int repeatTimes;
    int reqAckType;
    int requestID;
    uint64_t serialID;
};
#pragma pack(pop)

#define PackageHeadLen sizeof(PackageHeadField)