#include <iostream>
#include "DispatcherService.h"

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
    : threadPool(4), m_ctx(1), m_sockBind(m_ctx, ZMQ_ROUTER), m_sockInprocServer(m_ctx, ZMQ_ROUTER)
{
    //设置套接字接收、发送缓存队列，设置为0时表示无限制
    int hwm = 0;
    zmq_setsockopt(m_sockBind, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(m_sockBind, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(m_sockInprocServer, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(m_sockInprocServer, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    m_timerKA = Timer(10000);
    m_timerKA.registHandle(bind(&DispatcherService::katimeHandle, this));
    m_sockInprocServer.bind(INPROC_BIND_ADDRESS);
    m_bUpdManagerCache.store(true);
}

DispatcherService::~DispatcherService()
{
}

void DispatcherService::updManagerCache()
{
    char sql[128] = {0};
    sprintf(sql, "select * from ManageUser where 1=1");
    unique_lock<decltype(m_mtxSqlite)> sqliteLock(m_mtxSqlite);
    vector<ManageUser> vc = qryManageUserBySql(sql);
    sqliteLock.unlock();
    m_setManagerCache.clear();
    cout << "updManagerCache: " << vc.size() << endl;
    for (size_t i = 0; i < vc.size(); i++)
    {
        m_setManagerCache.insert(vc[i].ManagerName);
    }
}

void DispatcherService::pollFunc()
{
    while (1)
    {
        bool bTrue = true, bFalse = false;
        //manager-cache update
        if (m_bUpdManagerCache.compare_exchange_strong(bTrue, false))
        {
            updManagerCache();
        }

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
            memcpy(&cmd, message.data(), sizeof(int));
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
                innerSendMsg((void *)strRouter.c_str(), strRouter.length(), true);
                int msgType = STCMsgPattern::TPassiveResponse;
                innerSendMsg(&msgType, sizeof(msgType), true);
                innerSendMsg(&rsp, sizeof(rsp));
            }
            else
            {
                //find client
                auto it = m_mapClients.find(strIdentity);
                shared_ptr<DispatcherClient> client;
                if (it == m_mapClients.end())
                {
                    client.reset(new DispatcherClient);
                    client->m_strIdentify = strIdentity;
                    //client->m_strRouter = strRouter;
                    m_mapClients[strIdentity] = client;
                }
                else
                    client = it->second;
                //登录检测
                if (!client->m_bLogin && cmd != ECommandType::TLogin && cmd != ECommandType::TKeepAlive)
                {
                    int reqid{0};
                    m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                    if (more)
                        recvPodObject(message, reqid);
                    ReqResponse rsp;
                    rsp.CmdType = cmd;
                    rsp.ErrorID = EResponseErrType::TNoLogin;
                    rsp.RequestID = reqid;
                    strcpy(rsp.ErrMsg, "No-Login Error");
                    innerSendMsg((void *)strRouter.c_str(), strRouter.length(), true);
                    int msgType = STCMsgPattern::TPassiveResponse;
                    innerSendMsg(&msgType, sizeof(msgType), true);
                    innerSendMsg(&rsp, sizeof(rsp));
                    goto skip_label;
                }
                //同一登陆点检测
                if (client->m_bLogin && client->m_strRouter != strRouter)
                {
                    int reqid{0};
                    m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                    if (more)
                        recvPodObject(message, reqid);
                    ReqResponse rsp;
                    rsp.CmdType = cmd;
                    rsp.ErrorID = EResponseErrType::TMultiLogin;
                    rsp.RequestID = reqid;
                    strcpy(rsp.ErrMsg, "Multi-Login Error");
                    innerSendMsg((void *)strRouter.c_str(), strRouter.length(), true);
                    int msgType = STCMsgPattern::TPassiveResponse;
                    innerSendMsg(&msgType, sizeof(msgType), true);
                    innerSendMsg(&rsp, sizeof(rsp));
                    goto skip_label;
                }
                client->m_strRouter = strRouter;           //保持strRouter最新
                client->m_tLastRead = SYSTEM_CLOCK::now(); //更新lastRead时间戳
                onRecvCmd(cmd, client);
            }
            //忽略剩余帧
        skip_label:
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

void DispatcherService::innerSendMsg(void *data, int len, bool bMore)
{
    zmq_send(m_sockBind, data, len, bMore ? ZMQ_SNDMORE : 0);
}

zmq::socket_t DispatcherService::inProcSocket()
{
    zmq::socket_t socket(m_ctx, ZMQ_DEALER);
    int hwm = 0;
    zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    socket.connect(INPROC_BIND_ADDRESS);
    return socket;
}

void DispatcherService::outerSendMsg(zmq::socket_t &sndsocket, void *data, int len, bool bMore)
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
            innerSendMsg((void *)strRouted.c_str(), strRouted.length(), true);
            int msgType = STCMsgPattern::TActivePush;
            innerSendMsg(&msgType, sizeof(int), true);
            int cmd = ECommandType::TKeepAlive;
            innerSendMsg(&cmd, sizeof(int));
            // std::cout << "send keep-alive cmd" << std::endl;
        }
    }
}

void DispatcherService::onRecvCmd(int cmd, shared_ptr<DispatcherClient> client)
{
    zmq::message_t message;
    switch (cmd)
    {
    case ECommandType::TLogin:
        onReqLogin(client);
        break;
    case ECommandType::TLogout:
        onReqLogout(client);
        break;
    case ECommandType::TKeepAlive:
        onKeepAlive(client);
        break;
    case ECommandType::TQuery:
    {
        int reqid = 0;
        recvPodObject<int>(message, reqid);
        int eletype = 0;
        recvPodObject<int>(message, eletype);
        vector<ColumnFilter> colFilterVec;
        int more = 0;
        auto more_size = sizeof(more);
        ColumnFilter filter;
        while (1)
        {
            m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            if (!more)
                break;
            m_sockBind.recv(message);
            filter.ColName = string((char *)message.data(), message.size());
            m_sockBind.recv(message);
            filter.ColVal = string((char *)message.data(), message.size());
            colFilterVec.push_back(filter);
        }
        char ch[64] = {0};
        sprintf(ch, "select * from %s where 1=1", AllTableNames[eletype]);
        string sql = ch;
        for (int i = 0; i < colFilterVec.size(); i++)
        {
            memset(ch, 0, sizeof(ch));
            sprintf(ch, " and %s=\"%s\"",
                    colFilterVec[i].ColName.c_str(), colFilterVec[i].ColVal.c_str());
            sql += ch;
        }
        sql += ";";
        switch (eletype)
        {
        case EElementType::TManageUser:
            threadPool.submit(std::bind(&DispatcherService::onQryBySql<ManageUser>, this, client, std::move(sql), eletype, reqid));
            break;
        case EElementType::TServerConfig:
            threadPool.submit(std::bind(&DispatcherService::onQryBySql<ServerConfig>, this, client, std::move(sql), eletype, reqid));
            break;
        case EElementType::TStrategyConfig:
            threadPool.submit(std::bind(&DispatcherService::onQryBySql<StrategyConfig>, this, client, std::move(sql), eletype, reqid));
            break;
        case EElementType::TDeployConfig:
            threadPool.submit(std::bind(&DispatcherService::onQryBySql<DeployConfig>, this, client, std::move(sql), eletype, reqid));
            break;
        case EElementType::TDeployGroup:
            threadPool.submit(std::bind(&DispatcherService::onQryBySql<DeployGroup>, this, client, std::move(sql), eletype, reqid));
            break;
        default:
            break;
        }
    }
    break;
    case ECommandType::TDelete:
    {
        int reqid = 0;
        recvPodObject<int>(message, reqid);
        int eletype = 0;
        recvPodObject<int>(message, eletype);
        switch (eletype)
        {
        case EElementType::TManageUser:
        {
            vector<ManageUser> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onDelByObjects<ManageUser>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TServerConfig:
        {
            vector<ServerConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onDelByObjects<ServerConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TStrategyConfig:
        {
            vector<StrategyConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onDelByObjects<StrategyConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployConfig:
        {
            vector<DeployConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onDelByObjects<DeployConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployGroup:
        {
            vector<DeployGroup> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onDelByObjects<DeployGroup>, this, client, std::move(objVec), reqid));
        }
        break;
        default:
            skipRemainMFrame();
            break;
        }
    }
    break;
    case ECommandType::TInsert:
    {
        int reqid = 0;
        recvPodObject<int>(message, reqid);
        int eletype = 0;
        recvPodObject<int>(message, eletype);
        switch (eletype)
        {
        case EElementType::TManageUser:
        {
            vector<ManageUser> objVec;
            recvVecObject(message, objVec);
            m_bUpdManagerCache = true;
            threadPool.submit(std::bind(&DispatcherService::onInsByObjects<ManageUser>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TServerConfig:
        {
            vector<ServerConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onInsByObjects<ServerConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TStrategyConfig:
        {
            vector<StrategyConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onInsByObjects<StrategyConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployConfig:
        {
            vector<DeployConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onInsByObjects<DeployConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployGroup:
        {
            vector<DeployGroup> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onInsByObjects<DeployGroup>, this, client, std::move(objVec), reqid));
        }
        break;
        default:
            skipRemainMFrame();
            break;
        }
    }
    break;
    case ECommandType::TUpdate:
    {
        int reqid = 0;
        recvPodObject<int>(message, reqid);
        int eletype = 0;
        recvPodObject<int>(message, eletype);
        switch (eletype)
        {
        case EElementType::TManageUser:
        {
            vector<ManageUser> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onUpdByObjects<ManageUser>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TServerConfig:
        {
            vector<ServerConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onUpdByObjects<ServerConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TStrategyConfig:
        {
            vector<StrategyConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onUpdByObjects<StrategyConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployConfig:
        {
            vector<DeployConfig> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onUpdByObjects<DeployConfig>, this, client, std::move(objVec), reqid));
        }
        break;
        case EElementType::TDeployGroup:
        {
            vector<DeployGroup> objVec;
            recvVecObject(message, objVec);
            threadPool.submit(std::bind(&DispatcherService::onUpdByObjects<DeployGroup>, this, client, std::move(objVec), reqid));
        }
        break;
        default:
            skipRemainMFrame();
            break;
        }
    }
    break;
    case ECommandType::TDeploy:
        //TODO
        skipRemainMFrame();
        break;
    case ECommandType::TExecute:
        //TODO
        skipRemainMFrame();
        break;
    case ECommandType::TFinish:
        //TODO
        skipRemainMFrame();
        break;
    default:
        skipRemainMFrame();
        break;
    }
}

void DispatcherService::onReqLogin(shared_ptr<DispatcherClient> client)
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
    innerSendMsg((void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int msgType = STCMsgPattern::TPassiveResponse;
    innerSendMsg(&msgType, sizeof(int), true);
    innerSendMsg(&rsp, sizeof(rsp));
}

void DispatcherService::onReqLogout(shared_ptr<DispatcherClient> client)
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
    rsp.CmdType = ECommandType::TLogout;

    client->m_bLogin = false;
    rsp.ErrorID = EResponseErrType::TSuccess;
    strcpy(rsp.ErrMsg, "Logout Successfully");

    //////send response//////
    innerSendMsg((void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int msgType = STCMsgPattern::TPassiveResponse;
    innerSendMsg(&msgType, sizeof(int), true);
    innerSendMsg(&rsp, sizeof(rsp));
}

void DispatcherService::onKeepAlive(shared_ptr<DispatcherClient> client)
{
    client->m_tLastRead = SYSTEM_CLOCK::now();
    // std::cout << "receive keep-alive cmd" << std::endl;
}

bool DispatcherService::identifyUser(const string &strid)
{
    auto it = m_setManagerCache.find(strid);
    return it != m_setManagerCache.end();
}

void DispatcherService::skipRemainMFrame() //忽略剩余的消息帧
{
    zmq::message_t message;
    int more = 0;
    auto more_size = sizeof(more);
    while (1)
    {
        m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        if (!more)
            break;
        m_sockBind.recv(message);
    }
}
