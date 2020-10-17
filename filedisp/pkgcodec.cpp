#include "pkgcodec.h"

/////////////////PkgStreamInterface//////////////////////////
PkgStreamInterface::PkgStreamInterface()
    : objPtr(nullptr)
{
    pkgHead.isLast = true;
    pkgHead.pkgBodyLen = 0;
    pkgHead.pkgID = PKG_NONE;
    pkgHead.requestID = 0;
};
PkgStreamInterface::~PkgStreamInterface()
{
    if (nullptr != objPtr)
    {
        free(objPtr);
    }
};
void PkgStreamInterface::setHead(PkgHeadField *ptrPkgHead)
{
    pkgHead = *ptrPkgHead;
};
void PkgStreamInterface::setObject(void *pObj)
{
    switch (pkgHead.pkgID)
    {
    case PKG_LOGIN:
        objPtr = malloc(sizeof(LoginField));
        memcpy(objPtr, pObj, sizeof(LoginField));
        break;
    case PKG_LGOOUT:
        objPtr = malloc(sizeof(LogoutField));
        memcpy(objPtr, pObj, sizeof(LogoutField));
        break;
    case PKG_CHECKINFO:
        objPtr = malloc(sizeof(FileCheckInfoField));
        memcpy(objPtr, pObj, sizeof(FileCheckInfoField));
        break;
    case PKG_FILE:
        objPtr = malloc(sizeof(FileStreamField));
        memcpy(objPtr, pObj, sizeof(FileStreamField));
        break;
    case PKG_HEARTBEAT:
        objPtr = malloc(sizeof(HeartbeatField));
        memcpy(objPtr, pObj, sizeof(HeartbeatField));
        break;
    default:
        break;
    }
}

///////////////////////PkgCodec////////////////////////////////////
PkgCodec::PkgCodec()
    : headRecvd(0), bodyRecvd(0)
{
}
PkgCodec::~PkgCodec()
{
}
//字节解码，非线程安全，多个套接字之间不能共享
void PkgCodec::decode(BytesBuffer *buffer, std::vector<PkgStreamInterface> &decInteVec)
{
    int readPos = 0, nRemain, nread;
    void *destPtr, *srcPtr;
    int len = buffer->usedSize;
    char *chr = buffer->byteBuffer;
    while (readPos < len)
    {
        if (headRecvd < PKG_HEAD_LENGTH) //
        {
            nRemain = PKG_HEAD_LENGTH - headRecvd;
            nread = nRemain < len - readPos ? nRemain : len - readPos;
            destPtr = &pkgStruct.pkgHead + headRecvd;
            srcPtr = chr + readPos;
            memcpy(destPtr, srcPtr, nread);
            readPos += nread;
            headRecvd += nread;
        }
        else if (bodyRecvd < pkgStruct.pkgHead.pkgBodyLen)
        {
            nRemain = pkgStruct.pkgHead.pkgBodyLen - bodyRecvd;
            nread = nRemain < len - readPos ? nRemain : len - readPos;
            destPtr = pkgStruct.pkgData + bodyRecvd;
            srcPtr = chr + readPos;
            memcpy(destPtr, srcPtr, nread);
            readPos += nread;
            bodyRecvd += nread;
            if (bodyRecvd == pkgStruct.pkgHead.pkgBodyLen)
            {
                decInteVec.push_back(PkgStreamInterface());
                PkgStreamInterface &pkgStream = decInteVec[decInteVec.size() - 1];
                pkgStream.setHead(&pkgStruct.pkgHead);
                pkgStream.setObject(pkgStruct.pkgData);
                headRecvd = bodyRecvd = 0;
            }
        }
    }
    buffer->usedSize = 0;
}
//字节编码，非线程安全，多个套接字之间不能共享
void PkgCodec::encode(BytesBuffer *buffer, PkgStreamInterface *pkgStream)
{
    //超长，暂时不做任何处理
    if (pkgStream->pkgHead.pkgBodyLen + PKG_HEAD_LENGTH > buffer->totalSize)
        return;
    buffer->usedSize = 0;
    memcpy(buffer->byteBuffer + buffer->usedSize, &pkgStream->pkgHead, PKG_HEAD_LENGTH);
    buffer->usedSize += PKG_HEAD_LENGTH;
    memcpy(buffer->byteBuffer + buffer->usedSize, pkgStream->objPtr, pkgStream->pkgHead.pkgBodyLen);
    buffer->usedSize += pkgStream->pkgHead.pkgBodyLen;
};