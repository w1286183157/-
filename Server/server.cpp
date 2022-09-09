#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>
#include "../Hash/Hash.h"
#include "../RsaCrypto/RsaCrypto.h"
#include "../TcpSocket/TcpSocket.h"
#include "../TcpServer/TcpServer.h"
#include "../factory/RequestCodec.h"
#include "../factory/RespondCodec.h"

string getRandStr(int num)
{
    srand(time(NULL)); // 以当前时间为种子
    string retStr = string();
    char const *buf = "~`@#$%^&*()_+=-{}[];':";
    for (int i = 0; i < num; ++i)
    {
        int flag = rand() % 4;
        switch (flag)
        {
        case 0: // 0-9
            retStr.append(1, rand() % 10 + '0');
            break;
        case 1: // a-z
            retStr.append(1, rand() % 26 + 'a');
            break;
        case 2: // A-Z
            retStr.append(1, rand() % 26 + 'A');
            break;
        case 3: // 特殊字符
            retStr.append(1, buf[rand() % strlen(buf)]);
            break;
        }
    }
    return retStr;
}

string seckeyAgree(RequestMsg *msg)
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
    Cryptographic rsa("public.pem",false);
    Hash hash(T_SHA1);
    hash.addData(msg->data());
    bool b1 = rsa.rsaVerify(hash.result(), msg->sign());
    cout<<"签名验证结果 "<<b1<<endl;
    if (!b1)
    {
        info.status = false;
        cout << "签名校验失败！！！" << endl;
    }
    else
    {
        // 1.生成随机字符串
        string randStr = getRandStr(16);
        // 2.通过公钥加密
        string secStr = rsa.rsaPubKeyEncrypt(randStr);
        cout<<"生成的随机密钥:"<<secStr<<endl;
        // 3.初始化回复的数据
        info.status = true;
        info.seckeyID = 1;
        info.data = secStr;
        info.clientID = msg->clientid();
        info.serverID = msg->serverid();
    }
    // 4.序列化
    RespondCodec codec(&info);

    string data = codec.encodeMsg();
    return data;
}

void *connHandle(void *arg)
{
    TcpSocket *tcp = (TcpSocket *)arg;
    string msg = tcp->recvMsg();

    //反序列化 接收到的消息反序列化
    RequestCodec codec(msg);

    RequestMsg *reqMsg = (RequestMsg *)codec.decodeMsg();

    //取数据 根据请求判断

    string data;

    switch (reqMsg->cmdtype())
    {
    case 1:
        //密钥协商
        cout << "密钥协商" << endl;
        data = seckeyAgree(reqMsg);
        break;
    case 2:
        //密钥校验
        cout << "密钥校验" << endl;
        break;
    default:
        break;
    }
    tcp->sendMsg(data);
    return NULL;
}

int main()
{
    //读配置文件获取配置信息
    ifstream ifs("../assets/server_assets.json");
    assert(ifs.is_open());
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        cerr << "parse failed\n";
        return 1;
    }

    string serverID = root["ServerID"].asString();
    string port = root["Port"].asString();
    //启动服务器设置监听
    TcpServer *serv = new TcpServer;
    serv->setListen(atoi(port.data()));

    while (1)
    {
        cout << "等待客户端连接" << endl;
        TcpSocket *tcp = serv->acceptConn();
        if (tcp == NULL)
        {
            continue;
        }
        cout << "客户端连接成功" << endl;
        //创建子线程
        pthread_t tid;

        pthread_create(&tid, NULL, connHandle, (void *)tcp);
    }
}