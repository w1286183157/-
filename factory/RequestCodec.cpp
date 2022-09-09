#include "RequestCodec.h"

RequestCodec:: RequestCodec()
{
    cout << "asd" << endl;
}
RequestCodec::~RequestCodec()
{
  
}

RequestCodec::RequestCodec(string enc){
    initMessage(enc);
}

RequestCodec::RequestCodec(RequestInfo *info)
{
    initMessage(info);
}

void RequestCodec::initMessage(RequestInfo *info)
{
    reqMsg.set_clientid(info->clientID);
    reqMsg.set_serverid(info->serverID);
    reqMsg.set_cmdtype(info->cmdType);
    reqMsg.set_data(info->data);
    reqMsg.set_sign(info->sign);
}

void RequestCodec::initMessage(string enc){
    m_encstr=enc;
}

string RequestCodec::encodeMsg()
{
    string output;
    reqMsg.SerializeToString(&output);
    return output;
}

void *RequestCodec::decodeMsg()
{
    reqMsg.ParseFromString(m_encstr);
    return &reqMsg;
}
