#pragma once
#include <iostream>
#include "../ProtoBuf/Message.pb.h"
using namespace std;
struct RespondInfo
{
    int status;
    int seckeyID;
    string clientID;
    string serverID;
    string data;
};

class RespondCodec
{
public:
    RespondCodec();
    RespondCodec(RespondInfo *info);
    RespondCodec(string enc);
    ~RespondCodec();
    void initMessage(string enc);
    void initMessage(RespondInfo *info);
    string encodeMsg();
    void *decodeMsg();

private:
    string m_encstr;
    RespondMsg respMsg;
};