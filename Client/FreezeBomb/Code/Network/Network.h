#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "..\Stdafx\Stdafx.h"
#include "protocol.h"
#include <string>

#define SERVER_IP "127.0.0.1"
constexpr int SERVER_PORT = 9000;
constexpr int BUF_SIZE = 1024;


class Network
{
private:
	SOCKET	sock;
	int		myId;
	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;
public:
	void err_quit(const char *msg);
	void err_display(const char *msg);
public:
	Network();
	~Network();
public:
	SOCKET getSock();
	void connectToServer(HWND hWnd);
	void ReadPacket();
	void ProcessPacket(void* packet);
	void SendPacket(int data);
};

