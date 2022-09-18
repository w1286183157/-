#include "ServerOp.h"

ServerOP::ServerOP(string json)
{
    // 解析json文件, 读文件 -> Value
    //读配置文件获取配置信息
    ifstream ifs("../assets/server_assets.json");
    assert(ifs.is_open());
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        cerr << "json parse failed\n";
    }

    m_serverID = root["ServerID"].asString();
    m_port = root["Port"].asString();
    //数据库相关信息
    m_dbUser = root["UserDB"].asString();
    m_dbPwd = root["PwdDB"].asString();
    m_dbConnStr = root["ConnStrDB"].asString();
    m_dbset = root["DataSet"].asString();

    //实例化共享内存对象
    string shmKey = root["ShmKey"].asString();
    int maxNode = root["ShmMaxNode"].asInt();
    m_shm = new SecKeyShm(shmKey, maxNode);
    // 实例化一个连接myslq数据的对象
    m_mysql.connectDB(m_dbUser, m_dbPwd, m_dbset, m_dbConnStr);
}

void ServerOP::startServer()
{
    m_server = new TcpServer;
    m_server->setListen(atoi(m_port.c_str()));
    while (1)
    {
        cout << "等待客户端连接..." << endl;
        TcpSocket *tcp = m_server->acceptConn();
        if (tcp == NULL)
        {
            continue;
        }
        cout << "与客户端连接成功..." << endl;
        // 通信
        pthread_t tid;
        // 这个回调可以是类的静态函数, 类的友元函数, 普通的函数
        // 友元的类的朋友, 但是不属于这个类
        // 友元函数可以访问当前类的私有成员
        pthread_create(&tid, NULL, workHard, this);
        m_list.insert(make_pair(tid, tcp));
    }
}

void *ServerOP::working(void *arg)
{
    return nullptr;
}

string ServerOP::seckeyAgree(RequestMsg *msg)
{
    RespondInfo info;
    //密钥协商
    ofstream ofs("public.pem");
    assert(ofs.is_open());
    ofs << msg->data();

    ofs.flush();
    ofs.close();
    cout << "文件写入成功" << endl;

    // 1 检验签名
    RsaCrypto rsa("public.pem", false);
    Hash hash(T_SHA1);
    hash.addData(msg->data());
    bool b1 = rsa.rsaVerify(hash.result(), msg->sign());
    cout << "签名验证结果 " << b1 << endl;
    if (!b1)
    {
        info.status = false;
        cout << "签名校验失败！！！" << endl;
    }
    else
    {
        // 1.生成随机字符串
        string randStr = getRandKey(Len16);
        // 2.通过公钥加密
        string secStr = rsa.rsaPubKeyEncrypt(randStr);
        //写入到共享内存中
        cout << "生成的随机密钥:" << secStr << endl;
        // 3.初始化回复的数据
        info.status = true;

        info.data = secStr;
        info.clientID = msg->clientid();
        info.serverID = msg->serverid();
        //将生成的新密钥写入数据库
        NodeSHMInfo node;
        strcpy(node.clientID, msg->clientid().data());
        strcpy(node.serverID, msg->serverid().data());
        strcpy(node.seckey, randStr.data());
        node.seckeyID = m_mysql.getKeyID(); // 秘钥的ID
        info.seckeyID = node.seckeyID;
        
        node.status = 1;

        bool b = m_mysql.writeSecKey(&node);
        if (b1)
        {
            //成功
            m_mysql.updataKeyID(node.seckeyID + 1);
            //写共享内存
            NodeSecKeyInfo secKeyInfo;
            strcpy(secKeyInfo.clientID, msg->clientid().c_str());
            strcpy(secKeyInfo.serverID, msg->serverid().c_str());
            strcpy(secKeyInfo.seckey, randStr.data());
            secKeyInfo.seckeyID = node.seckeyID;
            m_shm->shmWrite(&secKeyInfo);
        }
        else
        {
            info.status = false;
        }
    }
    // 4.序列化
    RespondCodec codec(&info);
    string data = codec.encodeMsg();
    return data;
}

ServerOP::~ServerOP()
{
    if (m_server)
    {
        delete m_server;
    }
    delete m_shm;
}

// 要求: 字符串中包含: a-z, A-Z, 0-9, 特殊字符
string ServerOP::getRandKey(KeyLen len)
{
    // 设置随机数数种子 => 根据时间
    srand(time(NULL));
    int flag = 0;
    string randStr = string();
    char const *cs = "~!@#$%^&*()_+}{|\';[]";
    for (int i = 0; i < len; ++i)
    {
        flag = rand() % 4; // 4中字符类型
        switch (flag)
        {
        case 0: // a-z
            randStr.append(1, 'a' + rand() % 26);
            break;
        case 1: // A-Z
            randStr.append(1, 'A' + rand() % 26);
            break;
        case 2: // 0-9
            randStr.append(1, '0' + rand() % 10);
            break;
        case 3: // 特殊字符
            randStr.append(1, cs[rand() % strlen(cs)]);
            break;
        default:
            break;
        }
    }
    return randStr;
}

void *workHard(void *arg)
{
    string data = string();
    // 通过参数将传递的this对象转换
    ServerOP *op = (ServerOP *)arg;
    // 从op中将通信的套接字对象取出
    TcpSocket *tcp = op->m_list[pthread_self()];
    // 1. 接收客户端数据 -> 编码
    string msg = tcp->recvMsg();
    // 2. 反序列化 -> 得到原始数据 RequestMsg 类型
    RequestCodec codec(msg);
    RequestMsg *reqMsg = (RequestMsg *)codec.decodeMsg();
    // 3. 取出数据
    // 判断客户端是什么请求
    switch (reqMsg->cmdtype())
    {
    case 1:
        // 秘钥协商
        data = op->seckeyAgree(reqMsg);
        break;
    case 2:
        // 秘钥校验
        break;
    default:
        break;
    }

    // tcp对象如何处理
    tcp->sendMsg(data);
    tcp->disConnect();
    //貌似没有回收线程资源。。。。以后回收
    op->m_list.erase(pthread_self());
    delete tcp;
    return NULL;
}
