#pragma once
#include "Protocol.h"

class Network
{
public:
	Network();
	~Network();

	void err_quit(const char*);
	void err_display(const char*);
	int recvn(SOCKET, char*, int, int);

	bool Initialize();
	bool Packet_Initialize(byte);

	void Recv_InitPacket();
	void Recv_UpdatePacket();
	void SendPacket();

private:
#define SERVER_IP "127.0.0.1"
	//#define SERVER_IP "192.168.219.106"
	enum SERVER_INFO { SERVER_PORT = 9000, BUFSIZE = 1024 };
	
	SOCKET m_Server_socket;
};


