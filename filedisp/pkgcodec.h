#include <string.h>
#include <vector>
#include <memory>
#include "pkgcomstruct.h"

//包解码接口
class PkgStreamInterface
{
public:
    PkgHeadField pkgHead;
    void *objPtr; //转化后的对象指针

public:
    PkgStreamInterface();
    ~PkgStreamInterface();
    void setHead(PkgHeadField *ptrPkgHead);
    void setObject(void *pObj);
};

class PkgCodec
{
private:
    ////////解包相关////////////////////
    PkgStructField pkgStruct;
    int headRecvd; //已经接收的包头字节数
    int bodyRecvd; //已经接收的包体字节数
public:
    PkgCodec();
    ~PkgCodec();
    //字节解码，非线程安全，多个套接字之间不能共享
    void decode(BytesBuffer *buffer, std::vector<PkgStreamInterface> &decInteVec);
    //字节编码，非线程安全，多个套接字之间不能共享
    void encode(BytesBuffer *buffer, PkgStreamInterface *pkgStream);
};
