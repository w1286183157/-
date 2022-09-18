#pragma once
#include <mysql/mysql.h>
#include <iostream>
#include <string.h>
using namespace std;

class NodeSHMInfo
{
public:
    NodeSHMInfo() : status(0), seckeyID(0)
    {
        bzero(clientID, sizeof(clientID));
        bzero(serverID, sizeof(serverID));
        bzero(seckey, sizeof(seckey));
    }
    int status;        // 秘钥状态: 1可用, 0:不可用
    int seckeyID;      // 秘钥的编号
    char clientID[12]; // 客户端ID, 客户端的标识
    char serverID[12]; // 服务器ID, 服务器标识
    char seckey[128];  // 对称加密的秘钥
};

class MYSQLOP
{
private:
    MYSQL conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    string getCurTime();

public:
    MYSQLOP();
    ~MYSQLOP();
    bool connectDB(string user, string password, string database, string host);
    int getKeyID();
    bool updataKeyID(int keyID);
    //写密钥
    bool writeSecKey(NodeSHMInfo *pNode);
    void closeDB();
};