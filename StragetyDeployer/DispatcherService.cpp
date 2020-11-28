#include "DispatcherService.h"
#include "CommonStruct.h"

///////////////////DispatcherClient//////////////////////
DispatcherClient::DispatcherClient()
{
    m_tLastWrite = m_tLastRead = SYSTEM_CLOCK::now();
}

DispatcherClient::~DispatcherClient()
{
}

//////////////////////DispatcherService//////////////////////
DispatcherService::DispatcherService(/* args */)
    : threadPool(4)
{
    m_timerKA = Timer(10000);
    m_timerKA.registHandle(bind(&DispatcherService::katimeHandle, this));
    m_ctx = zmq::context_t(1);
    m_sockBind = zmq::socket_t(m_ctx, ZMQ_ROUTER);
    //thread-socket server
    m_sockInprocServer = zmq::socket_t(m_ctx, ZMQ_ROUTER);
    m_sockInprocServer.bind(INPROC_BIND_ADDRESS);
}

DispatcherService::~DispatcherService()
{
}

void DispatcherService::pollFunc()
{
    while (1)
    {
        //check-timer
        auto tnow = SYSTEM_CLOCK::now();
        if (m_timerKA.timeout(tnow))
        {
            m_timerKA.callback();
            m_timerKA.recount();
        }
        //poll
        zmq::pollitem_t items[] =
            {
                {m_sockInprocServer, 0, ZMQ_POLLIN, 0},
                {m_sockBind, 0, ZMQ_POLLIN, 0},
            };
        zmq::message_t message;
        zmq::poll(items, 2, 1000);
        if (items[0].revents & ZMQ_POLLIN)
        {
            int more;
            //skip router-id
            m_sockInprocServer.recv(message);
            while (1)
            {
                //real message contents
                m_sockInprocServer.recv(message);
                auto more_size = sizeof(more);
                m_sockInprocServer.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                m_sockBind.send(message, more ? ZMQ_SNDMORE : 0);
                if (!more)
                    break;
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            //router-id
            m_sockBind.recv(message);
            string strRouter((char *)message.data(), message.size());

            //manager-id
            m_sockBind.recv(message);
            string strIdentity((char *)message.data(), message.size());
            //cmd
            int cmd;
            m_sockBind.recv(message);
            //
            int more;
            auto more_size = sizeof(more);
            //鉴别用户是否存在
            if (!identifyUser(strIdentity))
            {
                int requestid{0};
                m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if (more)
                {
                    m_sockBind.recv(message);
                    memcpy(&requestid, message.data(), sizeof(requestid));
                }
                ReqResponse rsp;
                rsp.CmdType = cmd;
                rsp.ErrorID = EResponseErrType::TIdentifyErr;
                rsp.RequestID = requestid;
                strcpy(rsp.ErrMsg, "Identify Error");
                innerSendMsg(strRouter.c_str(), strRouter.length(), true);
                int msgType = STCMsgPattern::TPassiveResponse;
                innerSendMsg((const char *)&msgType, sizeof(msgType), true);
                innerSendMsg((const char *)&rsp, sizeof(rsp));
            }
            else
            {
                //find client
                auto it = m_mapClients.find(strRouter);
                shared_ptr<DispatcherClient> client;
                if (it == m_mapClients.end())
                {
                    client = make_shared<DispatcherClient>(new DispatcherClient);
                    client->m_strRouter = strRouter;
                    client->m_strIdentify = strIdentity;
                    m_mapClients[strIdentity] = client;
                }
                else
                {
                    client = it->second;
                }
                onRecvCmd(cmd, client);
            }
            //忽略剩余帧
            while (1)
            {
                m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if (!more)
                    break;
                m_sockBind.recv(message);
            }
        }
    }
}

void DispatcherService::innerSendMsg(const char *data, int len, bool bMore)
{
    zmq_send(m_sockBind, data, len, bMore ? ZMQ_SNDMORE : 0);
}

zmq::socket_t DispatcherService::inProcSocket()
{
    zmq::socket_t socket(m_ctx, ZMQ_DEALER);
    socket.connect(INPROC_BIND_ADDRESS);
    return socket;
}

void DispatcherService::outerSendMsg(zmq::socket_t &sndsocket, const char *data, int len, bool bMore)
{
    zmq_send(sndsocket, data, len, bMore ? ZMQ_SNDMORE : 0);
}

void DispatcherService::katimeHandle()
{
    auto tnow = SYSTEM_CLOCK::now();
    //遍历所有连接，keep-alive 检测
    for (auto it = m_mapClients.begin(); it != m_mapClients.end(); it++)
    {
        shared_ptr<DispatcherClient> client = it->second;
        if (client->m_tLastRead + chrono::minutes(1) < tnow)
        {
            client->m_bLogin = false;
        }
        else
        {
            client->m_tLastWrite = SYSTEM_CLOCK::now();
            //send keep alive cmd
            string strRouted = client->m_strRouter;
            innerSendMsg(strRouted.c_str(), strRouted.length(), true);
            int msgType = STCMsgPattern::TActivePush;
            innerSendMsg((char *)&msgType, sizeof(int), true);
            int cmd = ECommandType::TKeepAlive;
            innerSendMsg((char *)&cmd, sizeof(int));
        }
    }
}

void DispatcherService::onRecvCmd(int cmd, shared_ptr<DispatcherClient> &client)
{
    zmq::message_t message;
    switch (cmd)
    {
    case ECommandType::TLogin:
        onReqLogin(client);
        break;
    case ECommandType::TKeepAlive:
        onKeepAlive(client);
        break;
    case ECommandType::TQuery:
    case ECommandType::TInsert:
    case ECommandType::TDelete:
    case ECommandType::TUpdate:
    case ECommandType::TDeploy:
    case ECommandType::TExecute:
    case ECommandType::TFinish:
        //TODO
        break;
    default:
        break;
    }
}

void DispatcherService::onReqLogin(shared_ptr<DispatcherClient> &client)
{
    zmq::message_t message;
    //request id
    m_sockBind.recv(message);
    int reqid = 0;
    assert(message.size() == sizeof(int));
    memcpy(&reqid, (char *)message.data(), sizeof(int));

    ///////////////////////response///////////////////////
    ReqResponse rsp;
    rsp.RequestID = reqid;
    rsp.CmdType = ECommandType::TLogin;
    if (client->m_bLogin)
    {
        rsp.ErrorID = EResponseErrType::TReplicatedLogin;
        strcpy(rsp.ErrMsg, "Replicated Login");
    }
    else
    {
        client->m_bLogin = true;
        rsp.ErrorID = EResponseErrType::TSuccess;
        strcpy(rsp.ErrMsg, "Login Successfully");
    }
    //////send response//////
    innerSendMsg(client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int msgType = STCMsgPattern::TPassiveResponse;
    innerSendMsg((const char *)&msgType, sizeof(int), true);
    innerSendMsg((const char *)&rsp, sizeof(rsp));
}

void DispatcherService::onKeepAlive(shared_ptr<DispatcherClient> &client)
{
    client->m_tLastRead = SYSTEM_CLOCK::now();
}

bool DispatcherService::identifyUser(const string &strid)
{
    bool ret{false};
    //TODO
    return ret;
}
