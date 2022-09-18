#include "MYSQLOP.h"
#include <string.h>
MYSQLOP::MYSQLOP()
{
    if (mysql_init(&conn) == NULL)
    {
        cout << "init error,line:" << __LINE__ << endl;
        exit(-1);
    }
}
MYSQLOP::~MYSQLOP()
{
    //断开连接
    mysql_close(&conn);
}
bool MYSQLOP::connectDB(string user, string password, string database,string host)
{
    if (!mysql_real_connect(&conn, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0))
    {
        cout << "connect error,line" << __LINE__ << endl;
        return false;
    }
    return true;
}

int MYSQLOP::getKeyID()
{
    string querry = "select ikeysn from KEYSN for update";
    mysql_query(&conn, querry.c_str());

    result = mysql_store_result(&conn);
    int keyID = -1;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        keyID = atoi(row[0]);
    }
    return keyID;
}
bool MYSQLOP::updataKeyID(int keyID)
{
    string sql = "update KEYSN set ikeysn=" + to_string(keyID);
    int ResultNum;
    ResultNum = mysql_query(&conn, sql.c_str());

    int AffectROW = -1;

    AffectROW = (int)mysql_affected_rows(&conn);
    if (!ResultNum)
    {
        printf("update command [%s] exec successfully,affect row:%d row\n", sql.c_str(), AffectROW);
        return true;
    }
    else
    {
        printf("an error occured while insert data\n");
        return false;
    }
}

// 将生成的秘钥写入数据库
// 更新秘钥编号
bool MYSQLOP::writeSecKey(NodeSHMInfo *pNode)
{
    //组织要插入的sql语句
    char sql[1024]={0};

   sprintf(sql, "Insert Into SECKEYINFO(clientid, serverid, keyid, createtime, state, seckey) \
					values ('%s', '%s', %d, DATE_FORMAT('%s', '%%Y-%%m-%%d %%H:%%i:%%S') , %d, '%s') ", 
		pNode->clientID, pNode->serverID, pNode->seckeyID, 
		getCurTime().data(), 1, pNode->seckey);

    //printf("sql :%s\n",sql);
    int ResultNum;
    ResultNum = mysql_query(&conn, sql);

    int AffectROW = -1;

    AffectROW = (int)mysql_affected_rows(&conn);
    if (!ResultNum)
    {
        //printf("insert command [%s] exec successfully,affect row:%d row\n", sql, AffectROW);
        return true;
    }
    else
    {
        printf("an error occured while insert data\n");
        return false;
    }
}

string MYSQLOP::getCurTime(){
    time_t timep;
    time(&timep);
    char tmp[64];

    strftime(tmp,sizeof(tmp),"%Y-%m-%d %H:%M:%S",localtime(&timep));
    return tmp;
}
