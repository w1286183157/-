#pragma once
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

class BaseShm
{
public:
    BaseShm(int key);
    BaseShm(int key,int size);
    BaseShm(string name);
    BaseShm(string name,int size);
    ~BaseShm();
    void* mapShm();
    int unmapShm();
    int delShm();
private:
    int m_shmID;
protected:
    void *m_shmAddr;
private:
    int getShmId(key_t key,int shmSize,int flag);
};
