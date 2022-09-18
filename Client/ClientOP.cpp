#include"ClientOP.h"

ClientOP::ClientOP(string jsonFile)
{
	// 解析json文件, 读文件 -> Value
	ifstream ifs(jsonFile);
	assert(ifs.is_open());
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        cerr << "parse failed\n";
    }
	// 将root中的键值对value值取出
	m_info.ServerID = root["ClientID"].asString();
	m_info.ClientID = root["ServerID"].asString();
	m_info.ip = root["ServerIP"].asString();
	m_info.port = root["Port"].asString();

    string shmKey = root["ShmKey"].asString();
    int maxNode = root["ShmMaxNode"].asInt();

    m_shm=new SecKeyShm(shmKey,maxNode);
}
ClientOP::~ClientOP()
{
    delete m_shm;
}

bool ClientOP::seckeyAgree()
{
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
    reqMsg.clientID = m_info.ClientID;
    reqMsg.serverID = m_info.ServerID;
    reqMsg.cmdType = 1;                          //密钥协商
    reqMsg.data = str.str();                     //非对称加密公钥
    reqMsg.sign = crypto.rsaSign(hash.result()); //公钥的签名

    //将数据序列化 之后发送给服务器
    RequestCodec codec(&reqMsg);
    string encstr = codec.encodeMsg();


	// 套接字通信, 当前是客户端, 连接服务器
	TcpSocket* tcp = new TcpSocket;
	// 连接服务器
	int ret = tcp->connectToHost(m_info.ip, atoi(m_info.port.c_str()));
	if (ret != 0)
	{
		cout << "连接服务器失败..." << endl;
		return false;
	}
	cout << "连接服务器成功..." << endl;
	// 发送序列化的数据
	tcp->sendMsg(encstr);
	// 等待服务器回复
	string msg = tcp->recvMsg();

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

    //将密钥写入共享内存
    NodeSecKeyInfo info;
    strcpy(info.clientID,m_info.ClientID.c_str());
    strcpy(info.serverID,m_info.ServerID.c_str());
    strcpy(info.seckey,key_rec.data());
    info.seckeyID=respMsg->seckeyid();
    info.status=1;

    m_shm->shmWrite(&info);
    
	return true;
}
