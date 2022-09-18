#include "SecKeyShm.h"
#include <string.h>
#include <iostream>
using namespace std;

SecKeyShm::SecKeyShm(int key, int maxNode) : BaseShm(key, maxNode * sizeof(NodeSecKeyInfo)), m_maxNode(maxNode)
{
}
SecKeyShm::SecKeyShm(string pathName, int maxNode) : BaseShm(pathName, maxNode * sizeof(NodeSecKeyInfo)), m_maxNode(maxNode)
{
}
SecKeyShm::~SecKeyShm()
{
}
void SecKeyShm::shmInit()
{
    if (m_shmAddr != NULL)
    {
        memset(m_shmAddr, 0, m_maxNode * sizeof(NodeSecKeyInfo));
    }
}

int SecKeyShm::shmWrite(NodeSecKeyInfo *pNodeInfo)
{
    int ret = -1;
    //关联共享内存
    NodeSecKeyInfo *pAddr = static_cast<NodeSecKeyInfo *>(mapShm());
    if (pAddr == NULL)
    {
        return ret;
    }
    //判断传入网点密钥是否已经存在
    NodeSecKeyInfo *pNode = NULL;

    for (int i = 0; i < m_maxNode; i++)
    {
        pNode = pAddr + i;
        cout << "clientID 比较: " << pNode->clientID << ", " << pNodeInfo->clientID << endl;
        cout << "serverID 比较: " << pNode->serverID << ", " << pNodeInfo->serverID << endl;

        if (strcmp(pNode->clientID, pNodeInfo->clientID) == 0 &&
            strcmp(pNode->serverID, pNodeInfo->serverID) == 0)
        {
            // 如果找到了该网点秘钥已经存在, 使用新秘钥覆盖旧的值
            memcpy(pNode, pNodeInfo, sizeof(NodeSecKeyInfo));
            unmapShm();
            cout << "写数据成功: 原数据被覆盖!" << endl;
            return 0;
        }
    }
    int i;
    //如果没有找到，将一个空节点密钥信息写入
    NodeSecKeyInfo tmpNodeInfo;
    for (i = 0; i < m_maxNode; i++)
    {
        pNode = pAddr + i;
        if (memcmp(&tmpNodeInfo, pNode, sizeof(NodeSecKeyInfo)) == 0)
        {
            //找到了空节点
            ret = 0;
            memcpy(pNode, pNodeInfo, sizeof(NodeSecKeyInfo));
            cout << "写入数据成功：在新节点添加数据" << endl;
            break;
        }
    }

    if (i == m_maxNode)
    {
        ret = -1;
        cout << "m_maxNode is full" << endl;
    }
    unmapShm();
    return ret;
}

NodeSecKeyInfo SecKeyShm::shmRead(string clientID, string serverID)
{
    int ret = 0;
    //关联共享内存
    NodeSecKeyInfo *pAddr = NULL;
    pAddr = static_cast<NodeSecKeyInfo *>(mapShm());
    if (pAddr == NULL)
    {
        cout << "共享内存关联是被..." << endl;
        return NodeSecKeyInfo();
    }
    cout << "共享内存关联成功..." << endl;

    //便利寻找 要读的节点信息
    int i = 0;
    NodeSecKeyInfo info;
    NodeSecKeyInfo *pNode = NULL;
    //通过clientID和serverID查找节点
    for (i = 0; i < m_maxNode; i++)
    {
        pNode = pAddr + i;
        if (strcmp(pNode->clientID, clientID.data()) == 0 &&
            strcmp(pNode->serverID, serverID.data()) == 0)
        {
            //找到节点信息 传出参数
            info =*pNode;
            cout << "++++++++++++++" << endl;
			cout << info.clientID << " , " << info.serverID << ", "
				<< info.seckeyID << ", " << info.status << ", "
				<< info.seckey << endl;
			cout << "===============" << endl;
			break;
        }
    }
    unmapShm();
    return info;
}
