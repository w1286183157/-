#include"RespondCodec.h"

RespondCodec::RespondCodec(){

}

RespondCodec::RespondCodec(RespondInfo *info){
    initMessage(info);
}

RespondCodec::~RespondCodec(){

}

RespondCodec::RespondCodec(string enc){
    initMessage(enc);
}

void RespondCodec::initMessage(RespondInfo *info){
    respMsg.set_clientid(info->clientID);
    respMsg.set_serverid(info->serverID);
    respMsg.set_data(info->data);
    respMsg.set_seckeyid(info->seckeyID);
    respMsg.set_status(info->status);
}

void RespondCodec::initMessage(string enc){
    m_encstr=enc;
}

string RespondCodec::encodeMsg(){
    string output;
    respMsg.SerializeToString(&output);
    return output;
}

void* RespondCodec::decodeMsg(){
    respMsg.ParseFromString(m_encstr);
    return &respMsg;
}
