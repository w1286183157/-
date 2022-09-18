RSA_DIR=../RsaCrypto/
HASH_DIR=../Hash/
FACTORY_DIR=../factory/
PROTO_DIR=../ProtoBuf/
TCP_DIR=../TcpSocket/
SHM_DIR=../ShareMemory/

SRC=$(wildcard ./*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRC))

TCP_SRC=$(wildcard $(TCP_DIR)*.cpp)
TCP_OBJ=$(patsubst %.cpp,%.o,$(TCP_SRC))

PROTO_SRC=$(wildcard $(PROTO_DIR)*.cc)
PROTO_OBJ=$(patsubst %.cc,%.o,$(PROTO_SRC))

RSA_SRC=$(wildcard $(RSA_DIR)*.cpp)
RSA_OBJ=$(patsubst %.cpp,%.o,$(RSA_SRC))

HASH_SRC=$(wildcard $(HASH_DIR)*.cpp)
HASH_OBJ=$(patsubst %.cpp,%.o,$(HASH_SRC))

FACTORY_SRC=$(wildcard $(FACTORY_DIR)*.cpp)
FACTORY_OBJ=$(patsubst %.cpp,%.o,$(FACTORY_SRC))


SHM_SRC=$(wildcard $(SHM_DIR)*.cpp)
SHM_OBJ=$(patsubst %.cpp,%.o,$(SHM_SRC))

TARGET=client
CC=g++
LIBS=-lprotobuf -std=c++11 -lpthread -Bstatic -ljsoncpp  -lcrypto  

$(TARGET):$(OBJS)  $(HASH_OBJ) $(PROTO_OBJ) $(RSA_OBJ)  $(TCP_OBJ) $(FACTORY_OBJ) $(SHM_OBJ)
	$(CC) $^ -o $@  $(LIBS)
%.o: %.cpp
	$(CC) $< -c -o $@
$(FACTORY_DIR)%.o:$(FACTORY_DIR)%.cpp
	$(CC) $< -c -o $@  
$(PROTO_DIR)%.o:$(PROTO_DIR)%.cc
	$(CC) $< -c -o $@
$(RSA_DIR)%.o:$(RSA_DIR)%.cpp
	$(CC) $< -c -o $@  
$(HASH_DIR)%.o:$(HASH_DIR)%.cpp
	$(CC) $< -c -o $@  
$(TCP_DIR)%.o:$(TCP_DIR)%.cpp
	$(CC) $< -c -o $@  
$(SHM_DIR)%.o:$(SHM_DIR)%.cpp
	$(CC) $< -c -o $@  

.PHONY:clean
clean:
	rm -rf $(OBJS) $(OBJS2) $(RSA_OBJ) $(HASH_OBJ) $(FACTORY_OBJ) $(TCP_OBJ) $(SHM_OBJ)
