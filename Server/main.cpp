#include <cstdio>
#include "ServerOp.h"

int main()
{
	ServerOP op("server.json");
	op.startServer();

    return 0;
}