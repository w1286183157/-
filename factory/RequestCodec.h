#pragma once
#include <iostream>
#include "../ProtoBuf/Message.pb.h"
using namespace std;
struct RequestInfo
{
    int cmdType;
    string clientID;
    string serverID;
    string sign;
    string data;
};

class RequestCodec
{
public:
    RequestCodec();
    RequestCodec(RequestInfo *info);
    RequestCodec(string enc);
    void initMessage(string enc);
    void initMessage(RequestInfo *info);
    string encodeMsg();
    void *decodeMsg();
    ~RequestCodec();

private:
    string m_encstr;
    RequestMsg reqMsg;
};