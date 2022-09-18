#pragma once
#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>
#include "../Hash/Hash.h"
#include "../RsaCrypto/RsaCrypto.h"
#include "../TcpSocket/TcpSocket.h"
#include "../factory/RequestCodec.h"
#include "../factory/RespondCodec.h"
#include "../ShareMemory/SecKeyShm.h"
using namespace std;

struct ClientInfo
{
    string ServerID;
    string ClientID;
    string ip;
    string port;
};

class ClientOP
{
public:
    ClientOP(string jsonFile);
    ~ClientOP();

    // 秘钥协商
    bool seckeyAgree();

    // 秘钥校验
    void seckeyCheck() {}

    // 秘钥注销
    void seckeyZhuXiao() {}

private:
    ClientInfo m_info;
    SecKeyShm *m_shm;
};