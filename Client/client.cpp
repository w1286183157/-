#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>
#include "../Hash/Hash.h"
#include "../RsaCrypto/RsaCrypto.h"
#include "../TcpSocket/TcpSocket.h"
#include "../factory/RequestCodec.h"
#include "../factory/RespondCodec.h"

int _main()
{
    //读取配置文件
    ifstream ifs("../assets/client_assets.json");
    assert(ifs.is_open());
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        cerr << "parse failed\n";
        return 1;
    }
    string clientID = root["ClientID"].asString();
    string serverID = root["ServerID"].asString();
    string serverIP = root["ServerIP"].asString();
    string serverProt = root["Port"].asString();

    //生成非对称加密密钥
    RsaCrypto crypto;
    //生成密钥
    crypto.generateRsakey(1024, "public.pem", "private.pem");

    //获取非对称加密公钥
    ifstream key("public.pem");
    stringstream str;
    str << key.rdbuf();

    Hash hash(T_SHA1);
    hash.addData(str.str());

    //准备序列化发送数据
    RequestInfo reqMsg;
    reqMsg.clientID = clientID;
    reqMsg.serverID = serverID;
    reqMsg.cmdType = 1;                          //密钥协商
    reqMsg.data = str.str();                     //非对称加密公钥
    reqMsg.sign = crypto.rsaSign(hash.result()); //公钥的签名

    //将数据序列化 之后发送给服务器
    RequestCodec codec(&reqMsg);

    string encstr = codec.encodeMsg();

    //准备服务器套接字
    TcpSocket *tcp_clnt = new TcpSocket;
    //链接服务器
    cout << serverIP << ",端口: " << serverProt << endl;

    int ret = tcp_clnt->connectToHost(serverIP, atoi(serverProt.data()));
    if (ret != 0)
    {
        cout << "链接服务器失败。。。" << endl;
        return 0;
    }
    cout << "服务器连接成功...." << endl;

    //发送序列化之后的数据
    tcp_clnt->sendMsg(encstr);
    //等待服务器回复
    string msg = tcp_clnt->recvMsg();

    //解析服务器数据 -> 解码(反序列化)
    //数据还原
    RespondCodec codec_resp(msg);
    RespondMsg *respMsg=(RespondMsg*)codec_resp.decodeMsg();
    
    //判断状态
    if(!respMsg->status()){
        cout<<"密钥协商失败"<<endl;
        return false;
    }
    //将得到的密文解密
    string key_rec=crypto.rsaPriKeyDecrypt(respMsg->data());
    cout<<"对称加密的密钥: "<< key_rec<<endl;
    //写入到共享内存中 shm

    return 0;
}