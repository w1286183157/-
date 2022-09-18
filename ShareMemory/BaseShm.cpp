#include"BaseShm.h"

const char RandX='x';
BaseShm::BaseShm(int key){
    getShmId(key,0,0);
}

BaseShm::BaseShm(int key, int size){
    getShmId(key,size,IPC_CREAT|0664);
}

BaseShm::BaseShm(string name){
    key_t key=ftok(name.data(),RandX);
    getShmId(key,0,0);
}
BaseShm::BaseShm(string name, int size){
    key_t key=ftok(name.data(),RandX);
    getShmId(key,size,IPC_CREAT|0664);
}
BaseShm::~BaseShm(){

}
void * BaseShm::mapShm(){
    m_shmAddr=shmat(m_shmID,NULL,0);
    if (m_shmAddr == (void*)-1)
	{
		return NULL;
	}
	return m_shmAddr;
}
int BaseShm::unmapShm(){
    int ret=shmdt(m_shmAddr);
    return ret;
}
int BaseShm::delShm(){
    int ret=shmctl(m_shmID,IPC_RMID,NULL);
    return ret;
}
int BaseShm::getShmId(key_t key,int shmSize,int flag){
    cout << "share memory size: " << shmSize << endl;
	m_shmID = shmget(key, shmSize, flag);
	if (m_shmID == -1)
	{
		// 写log日志
		cout << "shmget 失败" << endl;
	}
	return m_shmID;
}